# 🎮 How To Play - AI Strategy Game

## 🚀 Running the Game

### In WSL (Linux on Windows):
```bash
export DISPLAY=:0
cd /mnt/c/Users/Anas/AI-Strategy-Game/build/Game
./AIStrategyGame
```

### Quick Launch (PowerShell):
```powershell
wsl bash -c "export DISPLAY=:0 && cd /mnt/c/Users/Anas/AI-Strategy-Game/build/Game && ./AIStrategyGame"
```

---

## 🎯 Controls

### Mouse Controls
- **Left Click** - Select a unit or building
- **Right Click** - Move selected unit to location (units only)
- **Close Window** - Exit game

### Keyboard
- *Coming soon!*

---

## 🎨 What You See

### Units (Circles with colors):
- **Yellow** - Workers (resource gatherers, fast, weak)
- **Green** - Soldiers (balanced combat units)
- **Gray** - Tanks (heavy, slow, strong)

### Buildings (Larger circles):
- **Blue** - Bases (spawn points, resource storage)
- **Orange** - Resource Mines (generate gold)
- **Red** - Turrets (defensive structures)

### Visual Feedback:
- **White Thick Outline** - Selected unit/building
- **Black Thin Outline** - Normal unit/building
- **White X/Cross** - Movement target destination
- **Cyan Dot in UI** - Unit is currently moving

---

## 📊 UI Panel (Bottom Left)

When you select a unit or building:
- **Colored Square** - Unit type indicator (matches unit color)
- **Health Bar** - 
  - Green = Healthy (60%+)
  - Yellow = Damaged (30-60%)
  - Red = Critical (<30%)
- **Cyan Dot** - Appears when unit is moving to target

---

## 🕹️ Gameplay Tips

### Selecting Units:
1. **Left-click** anywhere on a colored circle
2. Watch for **white outline** to confirm selection
3. Check **bottom-left UI panel** for unit info

### Moving Units:
1. Select a unit (Yellow/Green/Gray circles)
2. **Right-click** where you want it to go
3. Look for **white X** at destination
4. Watch unit move toward target
5. **Cyan dot** in UI shows it's moving

### Buildings:
- Can be **selected** (left-click)
- **Cannot move** (right-click does nothing)
- Show health and status in UI

---

## 🎬 Getting Started

**First Steps:**
1. Run the game
2. Left-click on a **yellow circle** (Worker) near top-left
3. See white outline appear = selected!
4. Right-click somewhere else on screen
5. Watch worker move to new location!
6. Try selecting different units

**Initial Setup:**
- **Top-left**: Player base + 2 workers + 1 soldier
- **Bottom-right**: Enemy base
- **Center**: 2 resource mines

---

## 🐛 Troubleshooting

**Gray screen only?**
- Units ARE rendered! Look for colored circles
- Try selecting near position (100, 100) - top-left area

**Nothing happens on click?**
- Check terminal for "Click at: X, Y" messages
- Make sure you're clicking ON the colored circles

**Unit won't move?**
- Buildings can't move (bases, mines, turrets)
- Only workers/soldiers/tanks can move
- Must select unit FIRST, then right-click

**"Setting vertical sync not supported"**
- This is just a warning, ignore it
- Game works perfectly fine!

---

## 🎯 Current Features

✅ **Working:**
- Unit selection with visual feedback
- Movement commands for units
- Health tracking and display
- Real-time movement system
- UI panel with unit info
- Multiple unit types

🚧 **Coming Soon:**
- Keyboard shortcuts
- Multiple unit selection (drag-box)
- Attack commands
- Building construction
- Resource collection
- AI opponents

---

## 📝 Notes

- Game window = 1200x800 pixels
- Target FPS = 60
- All positions in pixels
- Movement speed = 100 pixels/second (default)

Enjoy building your strategy! 🎮✨
