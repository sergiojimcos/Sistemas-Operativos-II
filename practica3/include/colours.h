/*********************************************
*   Project: Práctica 3 de Sistemas Operativos II 
*
*   Program name: colours.h
*
*   Author: Sergio Jiménez
*
*   Date created: 22-03-2020
*
*   Porpuse: Librería para dar formato a cada una de las palabras en la terminal del Sistema Operativo
*
*   Revision History: Reflejado en el repositorio de GitHub
|*********************************************/

#ifndef _COLORS_
#define _COLORS_

/* FOREGROUND */
#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"
#define YELLOW "\033[1;33m"
#define CIAN "\033[1;36m"
#define GREEN "\033[1;32m"
#define VIOLET "\033[1;35m"

#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST

#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST

#endif  /* _COLORS_ */