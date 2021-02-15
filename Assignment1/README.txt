To keep track of background jobs, I only really paid attention to the ones in slugRace, I used an array with the child
threads indexed to (id-1). This allowed me to iterate through them, and check them with a reliable structure.

In order to test the output, I just used the same seeds and tailored the majority of the programs to the output on the
project sheet.

Makefile commands

make all
    makes all the executibles

make clean
    removes all the .o files in the folder

make prolific
    makes the prolific executable

make generation
    makes the generation executable

make explorer
    makes the explorer executable

make slug
    makes the slug executable

make slugRace
    makes slug first, since slug is dependent
    then makes the slugRace executable, allowing for it to run without having to explicitly make slug first