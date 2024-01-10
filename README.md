Ce projet consiste à créer un programme en langage C qui utilise des threads POSIX pour observer les phénomènes d'exécution de tâches de manière parallèle. Le code sera développé pour une carte embarquée BeagleBone Black.

Il y a trois threads principaux dans ce projet :

Tstimer_thread : Un callback du thread du timer qui alterne l'état d'une ligne GPIO, signale un thread en attente et réinitialise le timer.

thread_adc : Lit la valeur du signal analogique (ADC) à partir d'un fichier système et stocke cette valeur dans une base de données SQLite.

thread_pwm : Génère un signal PWM en fonction de la valeur lue par le thread ADC.

La coordination entre ces threads est gérée pour assurer une exécution synchronisée. La base de données SQLite est utilisée pour stocker les valeurs ADC, avec une gestion robuste des erreurs assurée par des messages d'erreur affichés en cas de besoin.

Ce projet offre une opportunité d'acquérir des compétences dans la programmation parallèle avec des threads POSIX, la manipulation de signaux analogiques, la génération de signaux PWM, ainsi que la gestion de bases de données temps réel avec SQLite.







Thread 1 : Périodique, avec une période T de 100 ms. Envoie un message au début de chaque période et passe ensuite en repos.
Thread 2 : En repos, reçoit le signal du thread 1. Envoie la valeur numérique au thread 3 et passe ensuite en repos.
Thread 3 : Modifie le ratio du signal PWM et passe ensuite en repos.
