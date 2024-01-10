BE Système embarqué 
Ce projet consiste à créer un code en langage C comprenant des threads POSIX connectés les uns aux autres afin d'observer les phénomènes d'exécution des tâches de manière parallèle, en comprenant la théorie et la syntaxe qui les sous-tendent. Cela sera réalisé en développant 3 threads utilisant la carte embarquée BeagleBone Black.
Thread 1 : Périodique, avec une période T de 100 ms. Envoie un message au début de chaque période et passe ensuite en repos.
Thread 2 : En repos, reçoit le signal du thread 1. Envoie la valeur numérique au thread 3 et passe ensuite en repos.
Thread 3 : Modifie le ratio du signal PWM et passe ensuite en repos.
