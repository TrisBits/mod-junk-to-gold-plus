# <img src="icon.png" alt="Junk to Gold Plus icon" width="32" height="32"> Junk to Gold Plus

> This is a fork of the original [mod-junk-to-gold](https://github.com/noisiver/mod-junk-to-gold.git) module for [AzerothCore](https://github.com/azerothcore/azerothcore-wotlk.git).

Like the original, this module automatically sells gray (poor-quality) items when they are looted.

This fork adds safeguards to prevent auto-selling gray items that may still be useful.

Exceptions (items that will not be auto-sold):

- Quest items (including items that start a quest).
- Gray armor and weapons that are usable upgrades for the player's currently equipped gear.
- Gray items currently needed for an active quest objective.

## Installation

This module is intended for AzerothCore.

### Prerequisites

- A working AzerothCore source setup.
- Git installed.
- A successful baseline AzerothCore build before adding this module.

### 1. Go to your AzerothCore modules directory

From your AzerothCore root:

```bash
cd modules
```

### 2. Clone this module

```bash
git clone https://github.com/TrisBits/mod-junk-to-gold-plus.git
```

This will create:

```text
azerothcore/
  modules/
    mod-junk-to-gold-plus/
```

### 3. Build AzerothCore

Run this from the AzerothCore root directory (the directory that contains `acore.sh`):

```bash
./acore.sh compiler all
```

For future small updates, you can usually use a faster build-only command:

```bash
./acore.sh compiler build
```

### 4. Copy and edit the module config

The module ships with:

- `conf/mod_junk_to_gold_plus.conf.dist`

Copy it into your server config directory and remove `.dist` from the copied filename.

Common default config directory for source-based AzerothCore setups:

- `env/dist/etc`

Typical Linux example:

```bash
cp ../modules/mod-junk-to-gold-plus/conf/mod_junk_to_gold_plus.conf.dist ../env/dist/etc/mod_junk_to_gold_plus.conf
```

Typical Windows PowerShell example:

```powershell
Copy-Item ..\modules\mod-junk-to-gold-plus\conf\mod_junk_to_gold_plus.conf.dist ..\env\dist\etc\mod_junk_to_gold_plus.conf
```

Open the copied file and set options as needed.

### 5. Restart worldserver

Restart your worldserver so the module and configuration are loaded.

### 6. Verify it works

Log in with a character, loot a gray item, and confirm:

- The item is automatically sold.
- Money is credited correctly.
- Sale chat appears only when `JunkToGoldPlus.Enable = 1` and `JunkToGoldPlus.EnableSaleChat = 1`.

### Notes

- Do not run the original `mod-junk-to-gold` and this fork at the same time unless you intentionally want overlapping loot hooks.

## Updating

To update this module later, from your AzerothCore root run:

```bash
cd modules/mod-junk-to-gold-plus
git pull
cd ../..
./acore.sh compiler build
```

## Configuration

`JunkToGoldPlus.Enable` (default: `1`)

- `1`: Enable the module.
- `0`: Disable the module without removing it from your AzerothCore modules folder.

`JunkToGoldPlus.EnableSaleChat` (default: `1`)

- `1`: Send chat messages when gray items are auto-sold.
- `0`: Disable sale chat messages for quieter operation and slightly less overhead.
