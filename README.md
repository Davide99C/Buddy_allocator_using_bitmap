# Buddy_allocator_using_bitmap
# Progetto Sistemi Operativi 2020-2021
# Davide Chiarabini
# Buddy Allocator using bitmap
# What
Il progetto consiste nel realizzare un buddy allocator che riesce a gestire l’allocazione di
un buffer attraverso l’uso di una bitmap.
A tale scopo ho pensato alla bitmap come ad un albero binario completo dove il nodo radice
rappresenta il buffer completamente allocato in un unico buddy. Al livello 1, il buddy 1 e 2
rappresentano dei buddy di dimensione uguale alla metà del buffer e così via dimezzando ad ogni
livello la dimensione di quello precedente per ciascun nodo.
Inoltre per visualizzare meglio l’albero ho posto il nodo radice all’indice 0 modificando alcune delle
funzioni già fornite (il livello 0 ha radice di indice 0).
# How
Al fine di soddisfare una richiesta di allocazione cerco il livello con i blocchi più piccoli possibili e
che questi possano contenere il blocco di dimensione size da allocare, tenendo conto che lo spazio
richiesto sarà pari a size + sizeof(int), quest’ultimo per custodirci anche l’indice. In caso sia
disponibile il blocco che soddisfi tale richiesta ritorno il puntatore ad una porzione del buffer
assicurandomi di salvare l’indice del bit nella bitmap che rappresenta il buddy, all’interno dei
sizeof(int) di questo, altrimenti segnalo l’impossibilità attraverso una print e ritorno NULL.
Ogni volta che viene preso un blocco al livello desiderato vengono settati ad 1, (quindi occupati) i
bit di tutti i buddy padre, che avrebbero potuto originare il buddy che ho preso partizionandosi,
e quelli dei figli che sono ottenibili partizionandolo (ora il buddy è occupato e
non può piu’ essere partizionato per altri livelli).
Quando si fa la free di un blocco allocato si risale all’indice precedentemente nascosto poco prima
dell’indirizzo ritornato. Si controlla che l’indirizzo sia valido e che non si stia facendo una double
free, dopodichè si setta a 0 (quindi disponibile) lui e tutti i suoi figli. Nel caso in cui anche il suo
buddy sia libero si unisce e si rende libero anche il padre, questo ricorsivamente finché si arriverà
alla radice o il padre appena liberato abbia il buddy occupato.
# How-to-run
Per compilare il programma usare il makefile, verrà generato l’eseguibile buddy_allocator_test.
Lanciare l’esguibile con il comando ./buddy_allocator_test per vedere la gestione della memoria
(è anche possibile lanciare l’eseguibile con 1 o 2 argomenti, ovvero due interi per indicare la
dimensione del buffer dell’allocator, possibilmente una potenza di 2, ed il numero di livelli).
Il main è presente nel file buddy_allocator_test.c in cui viene inizializzato il buddy allocator e può
essere modificato a piacimento allocando e liberando blocchi pensando di utilizzare un allocatore
che gestisce l’heap.
