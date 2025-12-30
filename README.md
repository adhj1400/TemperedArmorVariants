# Tempered Armor Variants

A SKSE plugin that dynamically swaps armor addons based on the tempering level of equipped armor.

## Features

- Automatically detects tempering levels (Fine, Superior, Exquisite, Flawless, Epic, Legendary)
- Swaps armor addons based on JSON configuration files
- No scripts required - works entirely through SKSE hooks
- Lightweight and performance-friendly

## How It Works

When armor is equipped, the plugin:

1. Checks if the armor has registered variants in the config files
2. Reads the tempering level from the armor's ExtraHealth data
3. Replaces the armor addon with the configured variant for that tempering level

## Configuration

Place JSON config files in: `Data/SKSE/Plugins/TemperedArmorVariants/`

### JSON Format

```json
{
  "armors": [
    {
      "PluginName.esp|FormID": {
        "fine": "YourMod.esp|ArmorAddonFormID",
        "superior": "YourMod.esp|ArmorAddonFormID",
        "exquisite": "YourMod.esp|ArmorAddonFormID",
        "flawless": "YourMod.esp|ArmorAddonFormID",
        "epic": "YourMod.esp|ArmorAddonFormID",
        "legendary": "YourMod.esp|ArmorAddonFormID"
      }
    }
  ]
}
```

### Tempering Levels

The plugin uses Skyrim's ExtraHealth system:

- **Fine** = 1.1 health
- **Superior** = 1.2 health
- **Exquisite** = 1.3 health
- **Flawless** = 1.4 health
- **Epic** = 1.5 health
- **Legendary** = 1.6+ health

### Example Configuration

```json
{
  "armors": [
    {
      "Skyrim.esm|00013938": {
        "fine": "MyMod.esp|00000801",
        "superior": "MyMod.esp|00000802",
        "legendary": "MyMod.esp|00000806"
      }
    }
  ]
}
```

## Installation

1. Install SKSE
2. Copy `TemperedArmorVariants.dll` to `Data/SKSE/Plugins/`
3. Create configuration files in `Data/SKSE/Plugins/TemperedArmorVariants/`

## Finding FormIDs

Use xEdit (SSEEdit) or the Creation Kit to find:

- **Armor FormIDs**: The base armor item (ARMO records)
- **Armor Addon FormIDs**: The 3D models/addons (ARMA records)

FormIDs should be in hexadecimal format without the `0x` prefix.

## License

MIT
