from random_word import RandomWords
r = RandomWords()

# Return a single random word

arq = open("palavras.txt", "w+")


for a in range(150):
    
    arq.write(r.get_random_word()*5+"\n")

arq.close()