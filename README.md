# Projet Barbapapa - Carte électronique

Le Projet Barbapapa est un projet surprise qui réunit des anciens JINs.

L'une des parties de ce projet est un objet intéractif, ce dépôt en contient
la partie firmware conçue pour aller de pair avec
[sa partie électronique](https://github.com/Teo-CD/Projet_Barbapapa_PCB).

# Objectifs

Le firmware présent dans ce dépôt doit pouvoir contrôler les différents éléments
de la carte électronique :
 - Contrôle de puissance
   - Allumage et extinction du régulateur principal
   - Surveillance du niveau de la batterie (approximatif)
 - Contrôle des modules périphériques
   - LCD
   - RFID
   - (LED potentiellement)
 - Contrôle de l'amplificateur
   - Jouer des sons
   - Contrôle du gain

Les données utilisées pour l'affichage sur le LCD ou pour la sortie audio doivent
être chargés depuis une carte SD insérée dans la carte microcontrôleur.

En plus de contrôler la carte, le firmware doit pouvoir intéragir avec une
application développée sur Unity.  
En premier lieu, il doit renvoyer des identifiants correspondants aux tags RFID
détectés.

Sur le long terme, le logiciel doit pouvoir permettre de mettre à jour les tags,
changer les associations, potentiellement modifier les fichiers directement,
se synchroniser avec le firmware et potentiellement le mettre à jour.

# Statut

 - [ ] Contrôle de puissance
   - [x] Allumage et extinction du régulateur
   - [x] Contrôle minimal des modules
   - [ ] Gestion de la batterie
 - [ ] Contrôle des modules
   - [x] RFID (base)
   - [ ] RFID (avancé, écriture des tags)
   - [x] LCD (base)
   - [x] LCD (animations)
   - [ ] LCD (UI)
   - [x] Audio (sons de figurines)
   - [ ] Audio (sons d'UI)
   - [ ] Audio (gain ?)
 - [ ] Communication avec logiciel
   - [x] Retour basique des tags
   - [x] Communication HID automatique
   - [ ] Écriture possible des tags
 - [ ] Bonus - Aucun garantis
   - [ ] Synchronisation des listes
   - [ ] Mise à jour des fichiers
   - [ ] Mise à jour du firmware

# Pré-requis

Le code est relativement spécifique à la carte électronique, peu d'effort sera
réalisé pour le rendre flexible sur ce point.

## Pour compiler et mettre à jour la carte

Ce projet utilise [Platformio](https://platformio.org/) pour gérer la compilation,
mise à jour et suivi des dépendances.

Pour mettre à jour la carte, une fois connectée, vous pouvez lancer la commande suivante :  
```shell
platformio run -e prod -t upload
```

Pour utiliser l'environnement de développement et désactiver la communication HID, utilisez la commande suivante :  
```shell
platformio run -e dev -t upload
```

## Carte cible

La structure du code assume une carte Teensy. En particulier, il est conçu pour la
[Teensy 4.1](https://www.pjrc.com/store/teensy41.html).

# Carte SD et données

La carte SD est utilisée pour lire les animations et les sons, lors de l'utilisation
des figurines ou pour des cas génériques (démarrage, batterie faible…).  
Le firmware attend les fichiers dans certains endroits et sous certaines formes,
détaillés ici.

## Structure de fichiers

Les fichiers sur la carte SD sont structurés comme suit (**notez les majuscules**) :
```
/
├── RANDONNEE/
│   ├── ANIM/
│   │   ├── 00
│   │   ├── 01
│   │   ├── 02
│   │   ├── ..
│   │   └── XX
│   └── ENTRY.WAV
├── RESTO/
│   ├── ANIM/
│   │   └── ..
│   └── ENTRY.WAV
├── .../
└── SYS/
    ├── ANIM*/
    ├── ../
    └── *.WAV
```

Tous les fichiers liés à une figurine en particulier doivent être placés dans un
dossier associé au nom de la figurine.  
Les noms attendus sont ceux des tableaux définis dans `src/IDs.cpp`.  
Les différentes images d'une même animation doivent être dans un même dossier,
numérotés dans l'ordre avec deux chiffres et sans extension, les fichiers audio
doivent garder leur extension.

Le dossier SYS sera utilisé pour toute interaction qui n'est pas liée à une figurine.

## Structure des données
### Audio

Le format audio supporté est assez précis : les fichiers doivent être des WAV
16 bit PCM, 44100 Hz.  
Mono ou stéréo n'est pas important : la carte ne supporte qu'un haut-parleur,
les deux canaux stéréo seront moyennés `(Gauche+Droit)/2`.

### Animations

Le format des animations est d'autant plus précis et particulier et demandera
probablement du traitement pour qu'elles soient correctes.

Tout d'abord, chaque image de l'animation doit être dans son propre fichier, un
seul fichier pour l'animation (comme un gif) n'est pas possible.

L'écran LCD choisi est un module pour écran Nokia 5110 de 84x48 pixels monochromes,
les images de l'animation doivent donc être à ce format et monochromes.  
L'image attendue est exclusivement une bitmap d'un bit par pixel, sans aucune
autre information.  
De plus, il est probable que les bits attendus soient inversés par rapport à ceux
produits "logiquement" depuis un programme normal. Par exemple `0b11001100` devrait
en réalité être `0b00110011`.

Je n'ai pas connaissance d'un format connu qui peut être généré depuis un logiciel
et répondre à ces conditions, cependant il est possible d'y arriver automatiquement
avec quelques opérations de transformation.  
Pour la facilité, il est donc possible d'arriver aux fichiers individuels requis
à partir d'un simple gif aux proportions correctes.

À l'aide d'[ImageMagick](https://imagemagick.org/), on peut découper le gif en
ses images individuelles, et les convertir en un format de bitmap presque correct
(aux bits inversés) avec la commande suivante :

```shell
convert -resize 84x48 -coalesce <animation>.gif -threshold 50% %02d.mono
```

Il peut être nécessaire d'inverser les bits comme mentionné plus haut et je n'ai
pas trouvé de façon existante de le faire. 
Vous trouverez donc dans `Utils/bitmap_helper` un projet Rust qui permet de
réaliser cette opération sur les fichiers produits par la commande ci-dessus.  
Une fois compilé, vous pouvez convertir les fichiers `.mono` dans le format adapté
(avec une extension `.bin` en les passant à l'utilitaire :
```shell
bitmap_helper *.mono
```

N'oubliez pas de retirer l'extension et de les placer dans un dossier `ANIM` sur
la carte SD, comme décrit ci-dessus.

Un script bash est aussi fourni pour automatiser tout ce processus :
`Utils/convertAnimGif.sh`.

# Interfaçage avec l'application Unity

Afin de permettre la détection automatique de l'objet, la communication se fait
par Human Interface Device (HID).  
La communication HID est possible grâce aux bibliothèques de Teensyduino qui
fournissent des fonctionnalités de base.  
Ces fonctionnalités ne sont pas idéales et gâchent pas mal de bande passante,
en effet par défaut on ne peut qu'envoyer des rapports HIDs de 64 octets, malgré
le besoin de n'en consommer que quelques-uns dans le cas général.  
Faute de vouloir modifier les bibliothèques Teensyduino, nous nous contenterons
de ce gâchis, la Teensy étant absurdement rapide de toute façon.

## Protocole

Le protocole est constitué de messages avec un octet d'en-tête, puis d'un message
de longueur définie par l'en-tête, potentiellement variable jusqu'au maximum de
64 octets (avec en-tête).

### Changement de figurine

**En-tête**      : `@`  
**Longueur**     : 1 octet  
**Description**  : Message envoyé lors de la dépose ou du retrait d'une figurine.
Il contient l'ID de la figurine sur les bits 0-5 de l'octet, si la figurine
est un personnage (1) ou une année (0) sur le bit 6 et si la figurine a été posée
(0) ou retirée (1) sur le bit 7.  
Ainsi, si on interprète l'octet comme un nombre signé, si le nombre est négatif
on sait que la figurine a été retirée.

#### Schéma
```
Octet 1
┌─┬─┬───────────┐  
│D│C│ Figure ID │  
└─┴─┴───────────┘  
  7             0 - Bits
```
`D`: bit 7, 1 si enlevé, 0 sinon  
`C`: bit 6, 1 si personnage, 0 si année

### Requête d'information pour la programmation de tag

**En-tête**      : `?`  
**Longueur**     : 0 octets  
**Description**  : Message **vide** envoyé du logiciel vers l'objet pour
demander les informations complètes qui permettront de programmer les tags.  
Il attend une réponse de l'objet, décrit dans la catégorie suivante.

### Réponse d'information pour la programmation de tag

**En-tête**      : `!`  
**Longueur**     : 1 + 5*n octets, n le nombre de figurines présentes (>=0)  
**Description**  : Message de réponse à la requête d'information pour la programmation
des tags.  
Le premier octet est fixé et indique le nombre de figurines présentes, et donc
le nombre d'entrées dans le message.
Ensuite, chaque entrée est constituée de l'ID actuel, sur un octet, puis de l'UID
du tag, sur quatre octets.  
C'est l'UID qui est utilisé dans la demande de programmation pour sélectionner le tag.

#### Schéma

```
   │Optional, N records
 ┌─┼────┬──┬────┬──┐
 │N│UID1│I1│UID2│I2│...
 └─┴────┴──┴────┴──┘
 0 1    5  6    10 11 - Octets
```
`N`    : Nombre de tags présents, 1 octet  
`UIDn` : UID du tag n, 4 octets  
`In`   : ID du tag n, 1 octet  

### Message de programmation de tag

**En-tête**      : `=`  
**Longueur**     : 5 octets  
**Description**  : Message envoyé par le logiciel à l'objet, permet de changer
(programmer) l'ID stocké dans un tag.  
Le contenu est formaté de façon similaire à la réponse d'information :
l'UID du tag à programmer, sur quatre octets, puis le nouvel identifiant du tag,
sur un octet.  
On ne programme qu'un tag à la fois.

#### Schéma

```
 ┌─────┬────┐
 │ UID │ ID │
 └─────┴────┘
 0     4    5 - Octets
```
`UID` : UID du tag à programmer, 4 octets  
`ID`  : Nouvel ID à programmer, 1 octet  

### Message informatif

**En-tête**      : `#`  
**Longueur**     : 1+[1-62] octets  
**Description**  : Message générique à titre informatif, utile pour la communication
d'informations lors du développement ou de débug.  
Le premier octet est fixe et donne la longueur du reste du message, le message
est du texte encodé en ASCII.

#### Schéma
```
 ┌───┬── ── ── ──┐
 │Len│  Message  │
 └───┴── ── ── ──┘
 0   1         Len - Octets
```

# License

Les éléments dans ce dépôt sont Open Source, sous license
[Apache License V2.0](https://apache.org/licenses/LICENSE-2.0).
