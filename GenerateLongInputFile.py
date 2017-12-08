f = open("./test/test1.txt", "w+")
g = open("./test/test1_expectedOutput.txt", "w+")

for i in range(512, 0, -1):
	f.write("a" * i + "\n")
	g.write("a" * i + " : 1\n")

f.close()
g.close()