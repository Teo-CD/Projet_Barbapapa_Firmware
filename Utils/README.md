# Utilitaires

Ce sous-dossier rassemble divers utilitaires liés au projet.

# Conversion des animations

Deux outils sont disponnibles pour aider à la conversion : un pour faire l'inversion
de bit nécessaire, et un script pour automatiser tout le protocole.

Pour des détails sur le format de sortie, référez-vous au README principal.

## bitmap_helper

Un projet Rust a été créé pour permettre la conversion d'une bitmap MONO gérée
par ImageMagick en une bitmap correcte pour le LCD, en inversant les bits de chaque
octet.

Il est possible qu'une version compilée soit disponible dans les versions sur
Github.

### Compilation

Pour le compiler, il suffit d'utiliser Cargo, qui peut être installé via
[rustup](https://rustup.rs/) par exemple.  
Une fois Rust et Cargo disponibles, la compilation peut être faite depuis le
dossier `bitmap_helper` avec la commande suivante :
```shell
cargo build
```
L'exécutable produit est alors `bitmap_helper/target/debug/bitmap_helper`.

### Utilisation

L'utilitaire peut convertir plusieurs fichiers `.mono` à la fois, passés directement
sur la ligne de commande.  
```shell
bitmap_helper/target/debug/bitmap_helper <frame00>.mono <frame01>.mono ...
```
Tout autre format de fichier ne sera pas traité.  
⚠️ La détection du format n'est basée que sur les extensions !

## convertAnimGif.sh

Ce script permet d'automatiser tout le protocole de conversion d'un ou plusieurs
gifs en suites de bitmaps utilisables par le projet.

Il prend en argument un ou plusieurs fichier(s) `.gif` et produit les bitmap
individuelles correctement numérotées dans un dossier portant le nom du fichier
original.

```shell
./convertAnimGif.sh <animation1>.gif <animation2>.gif ...
```

Il suffit ensuite de déposer les dossiers sur la carte SD, sous les dossiers correspondants
(voir noms dans `src/IDs.cpp`) et les renommer pour ne laisser que `ANIM`.
