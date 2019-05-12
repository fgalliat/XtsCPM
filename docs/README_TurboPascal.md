# Usefull infos about TurboPascal....

**TP3 -> internal editor**

- can set terminal type w/ TINST.COM
- Ctrl K D to exit

**Exit (from subroutine or main Program)**

Exit exits the current subroutine, and returns control to the calling routine. If invoked in the main program routine, exit stops the program. The optional argument X allows to specify a return value, in the case Exit is invoked in a function. The function result will then be equal to X.

**Functions** (no return statment)

function Min (a,b:real) : real;
begin
    if a < b then Min := a else Min := b;
end;

**Includes**

 {$I MC-MOD00.INC        Miscelaneous procedures                     }

can handle comment @ right side...

& xxx.INC doesn't have to redefine some program headers or signatures

as real includes in some other languages



**Documentation CPC TP3**

<http://www.cpc-power.com/index.php?page=detail&onglet=notices&num=4183>

**Generalites**

from : <https://pascal.developpez.com/faq/?page=Turbo-Pascal-Generalites>

Les directives de compilation sont des options que vous définissez uniquement pour votre programme ou pour une partie de votre programme. Elles sont l'équivalent des panneaux sur le bord des routes, qui vous aident à vous diriger : les directives de compilation aident le compilateur et l'éditeur de liens à se diriger dans votre programme suivant vos souhaits.
Vous reconnaîtrez aisément une directive de compilation : comme les commentaires, elles sont écrites soit entre accolades *{ }* soit entre couples « parenthèse étoile » *(\* *)*. Toutefois, contrairement aux commentaires, elles commencent toujours par un signe dollar *$*.
**Attention !** Vous ne devez pas mettre d'espace entre l'accolade et le dollar, sans quoi votre directive de compilation sera prise pour un simple commentaire.

- **Directives à bascule**

Les directives à bascule activent ou désactivent des options du compilateur, selon le signe (+ ou -) qui suit le nom de la directive.

Voici les plus courantes :

| Directive   | Usage                                                        |
| ----------- | ------------------------------------------------------------ |
| {$A+} {$A-} | Alignement sur un word pour les variables et constantes typées - par défaut {$A+} |
| {$B+} {$B-} | Force une évaluation booléenne complète (non optimisée) - par défaut {$B-} |
| {$F+} {$F-} | Force un appel FAR (intersegment) - par défaut {$F-}         |
| {$I+} {$I-} | Provoque une erreur d'exécution en cas d'erreur d'entrée/sortie ({$I- permet d'intercepter l'erreur et d'utiliser IOResult) - par défaut {$I+} |
| {$Q+} {$Q-} | Génère des tests de débordement (overflow) dans certaines fonctions arithmétiques entières - par défaut {$Q-} |
| {$R+} {$R-} | Génère des tests de domaine de validité (longueurs de chaînes, intervalles) - par défaut {$R-} |
| {$S+} {$S-} | Génère un contrôle de l'espace disponible dans la pile pour la réservation des variables locales - par défaut {$S+} |
| {$V+} {$V-} | Effectue des contrôles sur la taille effective des chaînes de caractères - par défaut {$V+} |
| {$X+} {$X-} | Active la syntaxe étendue (utilisation de fonctions comme procédures et de chaînes à zéro terminal comme strings) - par défaut {$X+} |

- **Directives paramètres**

Les directives paramètres servent à définir des paramètres qui influent sur la compilation.

En voici les principales :

| Directive           | Usage                                                        |
| ------------------- | ------------------------------------------------------------ |
| {$I CONST.INC}      | Indique l'usage d'un fichier include (CONST.INC)             |
| {$L MESPROCS}       | Indique que l'éditeur de liens doit lier le fichier MESPROCS.OBJ |
| {$M 65520,0,655360} | Spécifie les paramètres d'allocation mémoire (taille de la pile et du tas) |
| {$R RESSOURC}       | Indique qu'il faut insérer les ressources contenues dans le fichier RESSOURC.RES dans l'exécutable (uniquement BPW et TPW, pour Windows et DPMI) |

- **Directives conditionnelles**

Les directives conditionnelles permettent de définir des blocs de texte source, dont la compilation dépendra de certaines conditions.

Voici les plus courantes :

| Directive                    | Usage                                                        |
| ---------------------------- | ------------------------------------------------------------ |
| {$DEFINE Condition}          | Émettre une condition                                        |
| {$UNDEF Condition}           | Supprimer une condition                                      |
| {$IFDEF Condition}           | Si Condition vérifiée alors compiler le bloc qui suit        |
| {$IFNDEF Condition}          | Si Condition non vérifiée alors compiler le bloc qui suit    |
| {$ELSE}                      | Définit un bloc *else* après une condition {$IFDEF} ou {$IFNDEF} |
| {$ENDIF}                     | Termine un bloc conditionnel {$IFDEF} ou {$IFNDEF}           |
| {$IFOPT Directive_à_bascule} | Si Directive_à_bascule dans l'état + ou - alors compiler le bloc qui suit |
| {$MSDOS}                     | Si DOS mode réel alors compiler le bloc qui suit             |
| {$DPMI}                      | Si mode DOS protégé alors compiler le bloc qui suit          |
| {$VER70}                     | Si n° de version de Turbo Pascal = 7.0 alors compiler le bloc qui suit |

| Code delphi : | [Sélectionner tout](https://pascal.developpez.com/faq/?page=Turbo-Pascal-Generalites#) |
| ------------- | ------------------------------------------------------------ |
|               |                                                              |

| 1 2 3 4 5 6 7 8 9 10 | `   {$DEFINE Compile}     {$IFDEF Compile}    Const                           { Ce code sera compilé }      Etat = 'Compilé !';  {$ELSE}    Const                           { Celui-ci ne le sera pas }      Etat = 'Pas compilé !';  {$ENDIF}` |
| -------------------- | ------------------------------------------------------------ |
|                      |                                                              |



Mis à jour le 3 janvier 2007	[Alcatîz](https://www.developpez.com/user/profil/13920/Alcatiz) [Eric Sigoillot

| Type     | Intervalle                      | Nombre d'octets |
| -------- | ------------------------------- | --------------- |
| ShortInt | -127 .. 128                     | 1               |
| Byte     | 0 .. 255                        | 1               |
| Integer  | -32 768 .. 32 767               | 2               |
| Word     | 0 .. 65 535                     | 2               |
| LongInt  | -2 147 483 648 .. 2 147 483 647 | 4               |