import torch
import torch.nn as nn
import torch.optim as optim
import chess
import chess.engine
import random
from tqdm import tqdm

# =====================
# 1. Architecture
# =====================
class MiniNNUE(nn.Module):
    def __init__(self):
        super().__init__()
        self.input = 768
        self.h1 = 16*512  # 8192
        self.h2 = 2*512
        self.out = 8

        self.fc1 = nn.Linear(self.input, self.h1)
        self.fc2 = nn.Linear(self.h1, self.h2)
        self.fc3 = nn.Linear(self.h2, self.out)

    def forward(self, x):
        x = torch.relu(self.fc1(x))
        x = torch.relu(self.fc2(x))
        x = self.fc3(x)
        # average the 8 outputs to one scalar
        return x.mean(dim=1, keepdim=True)

# =====================
# 2. FEN → feature encoding (768)
# =====================
PIECES = ['P','N','B','R','Q','K','p','n','b','r','q','k']
piece_to_idx = {p: i for i,p in enumerate(PIECES)}

def encode_fen(fen):
    board = chess.Board(fen)
    vec = torch.zeros(768)
    for sq in chess.SQUARES:
        piece = board.piece_at(sq)
        if piece:
            idx = piece_to_idx[piece.symbol()]
            vec[idx * 64 + sq] = 1.0
    return vec

# =====================
# 3. Training data generation
# =====================
def generate_training_data(n=2000, engine_path=r"C:\Users\kodxity\Downloads\stockfish-windows-x86-64-avx2\stockfish\stockfish-windows-x86-64-avx2.exe"):
    engine = chess.engine.SimpleEngine.popen_uci(engine_path)
    data = []
    for _ in tqdm(range(n), desc="Generating positions"):
        board = chess.Board()
        # make random moves for variety
        for _ in range(random.randint(5, 25)):
            if board.is_game_over():
                break
            move = random.choice(list(board.legal_moves))
            board.push(move)
        info = engine.analyse(board, limit=chess.engine.Limit(depth=10))
        score = info["score"].white().score(mate_score=10000) / 100.0  # scale
        fen = board.fen()
        data.append((fen, score))
    engine.quit()
    return data

# =====================
# 4. Training loop
# =====================
def train(model, data, epochs=3, batch_size=64):
    optimizer = optim.Adam(model.parameters(), lr=1e-3)
    loss_fn = nn.MSELoss()

    X = torch.stack([encode_fen(fen) for fen, _ in data])
    y = torch.tensor([[score] for _, score in data], dtype=torch.float32)

    for epoch in range(epochs):
        perm = torch.randperm(len(X))
        X, y = X[perm], y[perm]

        for i in range(0, len(X), batch_size):
            xb = X[i:i+batch_size]
            yb = y[i:i+batch_size]

            pred = model(xb)
            loss = loss_fn(pred, yb)

            optimizer.zero_grad()
            loss.backward()
            optimizer.step()

        print(f"Epoch {epoch+1}: loss={loss.item():.4f}")

# =====================
# 5. Export weights to .txt
# =====================
def export_weights(model, filename=r"engine\nn_weights1.txt"):
    with open(filename, "w") as f:
        # must match C++ order: w1,b1,w2,b2,w3,b3
        for layer in [model.fc1, model.fc2, model.fc3]:
            for w in layer.weight.detach().numpy().flatten():
                f.write(f"{w}\n")
            for b in layer.bias.detach().numpy().flatten():
                f.write(f"{b}\n")
    print(f"Saved weights to {filename}")

# =====================
# 6. Run everything
# =====================
if __name__ == "__main__":
    model = MiniNNUE()

    print("Generating data using Stockfish (takes a few minutes)...")
    data = generate_training_data(500)  # increase for stronger model

    print("Training...")
    train(model, data, epochs=5)

    export_weights(model)
 
