L = 64
mask = 2**L - 1

constant = 2**L//100 + 1
m = {}
mup = {}
for i in range(10000):
    high = (i * constant) >> L
    low = (i * constant) & mask
    key = low >> (L - 8)
    if key not in m:
        m[key] = set()
    m[key].add(i % 100)
    if(len(m[key]) > 1):
        print(key, len(m[key]), m[key])
        raise Exception("collision")
    keyhigh = high
    if keyhigh not in mup:
        mup[keyhigh] = set()
    mup[keyhigh].add(i//100)
    if(len(mup[keyhigh]) > 1):
        print(keyhigh, len(mup[keyhigh]), mup[keyhigh])
        raise Exception("key high collision")

for k in m:
    print(k, len(m[k]), m[k])

for k in mup:
   print(k, len(mup[k]), mup[k])

print(hex(constant))