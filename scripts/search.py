
# This script uses Z3, install it with:
# pip install z3-solver
from z3 import *

# Create a Z3 solver
solver = Solver()

# Define the 64-bit constant C
C = BitVec('C', 64)

# Test the constraint for X from 0 to 9999
for x in range(10000):
    # Compute the 64-bit product C * X
    product = C * ZeroExt(32, BitVecVal(x, 32))
    # Constraint: (C * X) >> 56 == X // 10000
    solver.add(Extract(63, 56, product) == x // 1000)
    solver.add(Extract(7, 0, product) == x % 10)

# Check if a solution exists
if solver.check() == sat:
    model = solver.model()
    print(f"Found constant C: {model[C].as_long()}")
    print(f"Hex: {hex(model[C].as_long())}")
else:
    print("No constant C satisfies the condition.")