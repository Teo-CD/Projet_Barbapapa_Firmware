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
   - [ ] LCD (base)
   - [ ] LCD (animations)
   - [ ] LCD (UI)
   - [ ] Audio (sons de figurines)
   - [ ] Audio (sons d'UI)
   - [ ] Audio (gain ?)
 - [ ] Communication avec logiciel
   - [x] Retour basique des tags
   - [ ] Communication HID automatique
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

## Carte cible

La structure du code assume une carte Teensy. En particulier, il est conçu pour la
[Teensy 4.1](https://www.pjrc.com/store/teensy41.html).

# License

Les éléments dans ce dépôts sont Open Source, sous license
[Apache License V2.0](https://apache.org/licenses/LICENSE-2.0).
