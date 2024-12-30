Το πρόγραμμα driver.c χρησημοποείται για να δημιουργήσει/καταστρέψει το shared memory segment και 
να τερματίσει την εκτέλεση του receptionist. Δεν δημιουργεί receptionist και visitors με fork()/exevp(),
χρησημοποιείται για την εκτέλεση σε διαφορετικά tty. 

Τα προγράμματα monitor.c, receptionist.c και visitor.c λειτουργούν με το τρόπο που περιγράφεται στην εκφώνηση.

Το εκτελέσιμο test που δημιουργείται απο το make τρέχει tests στη κύρια δομή που έχει υλοποιηθεί για την 
επίλυση του προβλήματος (circular buffer of semaphores) και στη σύνεχεια τρέχει ενα simulation του προβλήματος
για 30 δευτερόλεπτα το οποία αυτοματα δημιουργεί receptionist και visitors με fork()/execvp().

Για logging όσον αφόρα τους driver και receptionist δημιουργούνται τα αρχεία Receptionistlog.txt και
Driverlog.txt ενώ για τους visitors υπάρχει ένα flag έντος του κώδικα ώστε να εμφανίζεται το logging
στο εκάστωτε tty. 

Μετά την εκτέλεση του make όλα τα εκτελέσιμα βρίσκονται στον φάκελο build (υπάρχει make clean).

Όλες οι λειτουργίες και δομές του κάθε προγράμματος υλοποιούνται στο utils.c στο οποίο υπάρχει και το 
struct του shared memory segment. Έχει υλοποίηθει ένας circular buffer of semaphores 100 θέσεων στον οποίο
κάνει wait ο κάθε visitor μόλις φτάσει και περιμένει τον receptionist να τον κάνει post. υπάρχουν mutexes
για τις καρέκλες, για τον buffer και τα στατιστικά. Χρησημοποιούνται semamphores για αναμονή του receptionist
μέχρι να βρεί καρέκλα ο πελάτης και του driver μέχρι να ολοκληρώσει ο receptionist. Για την λήξη του προγράμματος
υπάρχει ένα flag.
