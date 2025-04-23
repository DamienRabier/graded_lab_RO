# Hackathon Operation Research

## Installation

### 1. Cloner le dépôt
Clonez le répertoire GitHub avec la commande suivante :
```bash
git clone https://github.com/DamienRabier/graded_lab_RO.git
```

### 2. Ajouter de nouvelles instances
- Placez les nouvelles instances dans le dossier `instances`.

#### Mise à jour des fichiers :
1. **`opt.txt`**  
   Ajoutez les valeurs `objective_value` correspondantes aux nouvelles instances.  
   Exemple : pour une nouvelle instance `tap_100_100.dat`, ajoutez la ligne suivante :  
   ```
   tap_100_100.dat;X
   ```

2. **`baselines.txt`**  
   Ajoutez les valeurs `epsilon_time` et `epsilon_distance` des nouvelles instances.  
   Exemple : pour une nouvelle instance `tap_100_100.dat`, ajoutez la ligne suivante :  
   ```
   tap_100_100.dat;X;X
   ```

3. **`baselines_results.txt`**  
   Ajoutez les valeurs `objective_value` correspondantes aux nouvelles instances.  
   Exemple : pour une nouvelle instance `tap_100_100.dat`, ajoutez la ligne suivante :  
   ```
   tap_100_100.dat;X
   ```

### 3. Exécution
Pour lancer le code, ouvrez un terminal à l'emplacement du dossier et exécutez la commande suivante :
```bash
make run
```