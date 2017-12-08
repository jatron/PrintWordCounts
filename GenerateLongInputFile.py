f = open("./test/test2.txt", "w+")
g = open("./test/test2_expectedOutput.txt", "w+")

for i in range(1024, 0, -1):
	f.write("a" * i + "\n")
	g.write("a" * i + " : 1\n")

f.close()
g.close()