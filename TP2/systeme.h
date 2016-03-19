
#ifndef __SYSTEM_H
#define __SYSTEM_H

#define MAX_PROCESS     (5)     /* nb maximum de processus  */

#define EMPTY           (0)     /* processus non-pret       */
#define READY           (1)     /* processus pret           */
#define SLEEPING        (2)     /* processus endormi        */
#define GETCHAR         (3)     /* processus endormi en attente de caractère */

/**********************************************************
 ** Codes associes aux appels systèmes
 ***********************************************************/

#define SYSC_EXIT       (0)
#define SYSC_PUTI       (1)
#define SYSC_NEW_THREAD (2)
#define SYSC_SLEEP      (3)
#define SYSC_GETCHAR    (4)
#define SYSC_FORK       (5)

/**********************************************************
** appel du systeme
***********************************************************/

PSW systeme(PSW m);

/**********************************************************
 ** proccess
 ***********************************************************/

void init_proccess();
int search_free_process();
void backup_process();

PSW ordonnanceur(PSW m);

#endif

