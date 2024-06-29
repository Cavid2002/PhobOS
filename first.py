pr = 0 
pb = 7
n = 77

while True:
    if (pr * pb) % n == 1:
        break
    pr += 1

print(pr)