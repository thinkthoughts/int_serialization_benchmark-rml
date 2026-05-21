from pathlib import Path
import yaml

CONFIG_DIR = Path("configs")

def load_yaml(path):
    with open(path, "r") as f:
        return yaml.safe_load(f)

print("Loaded config utilities.")
