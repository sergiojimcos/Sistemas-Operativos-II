DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -ansi
LDLIBS := -lpthread -lrt
CC := gcc

all : dirs manager pa pb pc pd

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

manager: $(DIROBJ)manager.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

pa: $(DIROBJ)pa.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

pb: $(DIROBJ)pb.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

pc: $(DIROBJ)pc.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

pd: $(DIROBJ)pd.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)


$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

run:
	./$(DIREXE)manager 

solution:
	./$(DIREXE)manager 2 3 4

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~
	rm -rf 0*
	rm -rf 55*
	rm -rf 75*
	rm -rf Estudiantes
	rm src/log.txt
	
