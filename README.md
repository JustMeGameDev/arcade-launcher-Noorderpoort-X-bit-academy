# arcade launcher Noorderpoort X bit academy
 this is a application build in C that functions as a arcade launcher for the arcade pc from Noorderpoort software development X bit academy

# Arcade Launcher - Game Toevoegen Handleiding

Deze handleiding beschrijft hoe je een nieuwe game toevoegt aan de Arcade Launcher.

## Vereisten

Elke game moet een eigen map hebben met de volgende structuur:

- `logo.png` - Het logo van de game dat in de launcher wordt getoond.
- `banner.png` - Een banner die bovenaan de informatiepagina wordt weergegeven.
- `info.txt` - Een tekstbestand met een korte beschrijving van de game.
- `credits.txt` - Een tekstbestand met de credits of makers van de game.
- `[game_name].exe` - Het uitvoerbare bestand voor de game.

> **Opmerking**: De mapnaam van de game en de naam van het uitvoerbare bestand moeten exact hetzelfde zijn.

## Stappen voor het Toevoegen van een Game


0. **Arcade launcher afsluiten**

    - Open de achterkant van de arcade kast en pak het toetsenbord.
    - drup op `BACKSPACE` om de applicatie af te sluiten.

1. **Game-map aanmaken**:  
   - Maak de volgende map aan; `Arcade Games` op de Desktop. Als de map al bestaat, kun je deze stap overslaan.
   - Maak een map aan binnen `Arcade Games` met de naam van de game. Bijvoorbeeld, als je game "SpaceInvaders" heet, maak je de map `Desktop/Arcade Games/SpaceInvaders`.

2. **Voeg het logo en de banner toe**:  
   - Plaats een afbeelding genaamd `logo.png` in de map van de game.
   - Plaats een afbeelding genaamd `banner.png` in de map van de game.

3. **Maak de beschrijvingsbestanden**:  
   - Voeg een `info.txt` bestand toe met een korte beschrijving van de game.
   - Voeg een `credits.txt` bestand toe met informatie over de makers.

4. **Voeg het uitvoerbare bestand toe**:  
   - Voeg het uitvoerbare bestand (`[game_name].exe`) toe in dezelfde map. De naam van het bestand moet overeenkomen met de naam van de map en van de game.

Door deze stappen te volgen, zorg je ervoor dat elke nieuwe game goed wordt geladen in de Arcade Launcher en dat alle elementen correct worden weergegeven.

---

# Game Developen voor de Arcade

In deze sectie leggen we de controls van de arcade setup uit. Dit is handig voor het ontwikkelen van games die naadloos werken met de arcadebediening.

### Arcade Controls

Hieronder vind je de afbeelding van de arcadebediening, die de verschillende knoppen en functies toont voor zowel Player 1 als Player 2:

![Arcade Controls](./README%20src/control%20sceme.png)

### Toelichting van de Controls

- **Player 1**:
  - **Joystick**: Bestuurt de beweging van de speler met de pijltoetsen.
  - **Knoppen 1-6**: Uitgevoerd door de genummerde knoppen 1 t/m 6 voor acties.
  
- **Player 2**:
  - **Joystick**: Bestuurt de beweging van de speler met de WASD-toetsen.
  - **Knoppen Z, X, C, V, B, N**: Toegekend aan specifieke acties.

- **Spelermodi Selectie**:
  - **O**: Selecteert de modus voor één speler.
  - **I**: Selecteert de modus voor twee spelers (coöperatieve modus).

- **Extra bediening**:
  - **Linker Muisklik**: Toegewezen aan een specifieke arcadeknop.
  - **Rechter Muisklik**: Toegewezen aan een specifieke arcadeknop.
  - **Muisbeweging**: Wordt bediend door een grote bal voor navigatie of andere interacties binnen de game.

Door deze controls in je game te integreren, zorg je voor een soepele arcade-ervaring voor beide spelers.

