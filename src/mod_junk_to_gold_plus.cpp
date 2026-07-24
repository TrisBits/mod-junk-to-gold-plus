#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ScriptMgr.h"

namespace
{
    static char const* const ITEM_LINK_MULTIPLE = "|cff9d9d9d|Hitem:{}::::::::{}:::::|h[{}]|h|rx{}";
    static char const* const ITEM_LINK_SINGLE = "|cff9d9d9d|Hitem:{}::::::::{}:::::|h[{}]|h|r";

    static char const* const SOLD_FOR_COPPER = "{} sold for {} copper.";
    static char const* const SOLD_FOR_SILVER_COPPER = "{} sold for {} silver and {} copper.";
    static char const* const SOLD_FOR_SILVER = "{} sold for {} silver.";
    static char const* const SOLD_FOR_GOLD_SILVER_COPPER = "{} sold for {} gold, {} silver and {} copper.";
    static char const* const SOLD_FOR_GOLD_COPPER = "{} sold for {} gold and {} copper.";
    static char const* const SOLD_FOR_GOLD_SILVER = "{} sold for {} gold and {} silver.";
    static char const* const SOLD_FOR_GOLD = "{} sold for {} gold.";
}

class JunkToGoldPlus : public PlayerScript
{
public:
    JunkToGoldPlus() : PlayerScript("JunkToGoldPlus") {}

    void OnPlayerLootItem(Player* player, Item* item, uint32 count, ObjectGuid /*lootguid*/) override
    {
        if (!IsModuleEnabled())
        {
            return;
        }

        if (!item)
        {
            return;
        }

        ItemTemplate const* itemTemplate = item->GetTemplate();
        if (!itemTemplate)
        {
            return;
        }

        if (itemTemplate->Quality != ITEM_QUALITY_POOR)
        {
            return;
        }

        if (IsQuestItem(itemTemplate))
        {
            return;
        }

        if (ShouldKeepAsUpgrade(player, itemTemplate))
        {
            return;
        }

        if (player->HasQuestForItem(itemTemplate->ItemId))
        {
            return;
        }

        if (IsSaleChatEnabled())
        {
            SendTransactionInformation(player, item, count);
        }

        player->ModifyMoney(itemTemplate->SellPrice * count);
        player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
    }

private:
    static bool IsModuleEnabled()
    {
        // Cache once to avoid repeated config lookups in a frequent hook.
        static bool const enableModule = sConfigMgr->GetOption<bool>("JunkToGoldPlus.Enable", true);
        return enableModule;
    }

    static bool IsSaleChatEnabled()
    {
        // Cache once to avoid repeated config lookups in a frequent hook.
        static bool const enableSaleChat = sConfigMgr->GetOption<bool>("JunkToGoldPlus.EnableSaleChat", true);
        return enableSaleChat;
    }

    static bool IsAllowedForPlayerClass(Player* player, ItemTemplate const* itemTemplate)
    {
        // -1 means usable by all classes in item templates.
        if (itemTemplate->AllowableClass == -1)
        {
            return true;
        }

        uint32 playerClassMask = 1u << (player->getClass() - 1u);
        return (itemTemplate->AllowableClass & playerClassMask) != 0;
    }

    static bool CanPlayerUseItem(Player* player, ItemTemplate const* itemTemplate)
    {
        return player->CanUseItem(itemTemplate) == EQUIP_ERR_OK;
    }

    static ItemTemplate const* GetEquippedTemplate(Player* player, uint8 slot)
    {
        if (Item* equipped = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        {
            return equipped->GetTemplate();
        }

        return nullptr;
    }

    static bool IsUpgradeForSlot(Player* player, ItemTemplate const* itemTemplate, uint8 slot)
    {
        ItemTemplate const* equippedTemplate = GetEquippedTemplate(player, slot);
        if (!equippedTemplate)
        {
            return true;
        }

        return itemTemplate->ItemLevel > equippedTemplate->ItemLevel;
    }

    static bool IsUpgradeForEitherSlot(Player* player, ItemTemplate const* itemTemplate, uint8 slotA, uint8 slotB)
    {
        return IsUpgradeForSlot(player, itemTemplate, slotA) || IsUpgradeForSlot(player, itemTemplate, slotB);
    }

    static bool ShouldKeepAsUpgrade(Player* player, ItemTemplate const* itemTemplate)
    {
        if (itemTemplate->Class != ITEM_CLASS_ARMOR && itemTemplate->Class != ITEM_CLASS_WEAPON)
        {
            return false;
        }

        if (!IsAllowedForPlayerClass(player, itemTemplate))
        {
            return false;
        }

        bool isUpgrade = false;

        switch (itemTemplate->InventoryType)
        {
            case INVTYPE_HEAD:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_HEAD);
                break;
            case INVTYPE_NECK:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_NECK);
                break;
            case INVTYPE_SHOULDERS:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_SHOULDERS);
                break;
            case INVTYPE_BODY:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_BODY);
                break;
            case INVTYPE_CHEST:
            case INVTYPE_ROBE:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_CHEST);
                break;
            case INVTYPE_WAIST:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_WAIST);
                break;
            case INVTYPE_LEGS:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_LEGS);
                break;
            case INVTYPE_FEET:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_FEET);
                break;
            case INVTYPE_WRISTS:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_WRISTS);
                break;
            case INVTYPE_HANDS:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_HANDS);
                break;
            case INVTYPE_FINGER:
                isUpgrade = IsUpgradeForEitherSlot(player, itemTemplate, EQUIPMENT_SLOT_FINGER1, EQUIPMENT_SLOT_FINGER2);
                break;
            case INVTYPE_TRINKET:
                isUpgrade = IsUpgradeForEitherSlot(player, itemTemplate, EQUIPMENT_SLOT_TRINKET1, EQUIPMENT_SLOT_TRINKET2);
                break;
            case INVTYPE_CLOAK:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_BACK);
                break;
            case INVTYPE_WEAPON:
                isUpgrade = IsUpgradeForEitherSlot(player, itemTemplate, EQUIPMENT_SLOT_MAINHAND, EQUIPMENT_SLOT_OFFHAND);
                break;
            case INVTYPE_2HWEAPON:
            case INVTYPE_WEAPONMAINHAND:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_MAINHAND);
                break;
            case INVTYPE_SHIELD:
            case INVTYPE_HOLDABLE:
            case INVTYPE_WEAPONOFFHAND:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_OFFHAND);
                break;
            case INVTYPE_RANGED:
            case INVTYPE_RANGEDRIGHT:
            case INVTYPE_THROWN:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_RANGED);
                break;
            case INVTYPE_RELIC:
                isUpgrade = IsUpgradeForSlot(player, itemTemplate, EQUIPMENT_SLOT_RANGED);
                break;
            default:
                return false;
        }

        if (!isUpgrade)
        {
            return false;
        }

        return CanPlayerUseItem(player, itemTemplate);
    }

    static bool IsQuestItem(ItemTemplate const* itemTemplate)
    {
        return itemTemplate->Class == ITEM_CLASS_QUEST || itemTemplate->StartQuest != 0;
    }

    void SendTransactionInformation(Player* player, Item* item, uint32 count)
    {
        // Keep string templates centralized for easier localization migration.
        ItemTemplate const* itemTemplate = item->GetTemplate();
        uint8 linkLevel = player->GetLevel();
        std::string name;
        if (count > 1)
        {
            name = Acore::StringFormat(ITEM_LINK_MULTIPLE, itemTemplate->ItemId, linkLevel, itemTemplate->Name1, count);
        }
        else
        {
            name = Acore::StringFormat(ITEM_LINK_SINGLE, itemTemplate->ItemId, linkLevel, itemTemplate->Name1);
        }

        uint32 money = itemTemplate->SellPrice * count;
        uint32 gold = money / GOLD;
        uint32 silver = (money % GOLD) / SILVER;
        uint32 copper = (money % GOLD) % SILVER;

        std::string info;
        if (money < SILVER)
        {
            info = Acore::StringFormat(SOLD_FOR_COPPER, name, copper);
        }
        else if (money < GOLD)
        {
            if (copper > 0)
            {
                info = Acore::StringFormat(SOLD_FOR_SILVER_COPPER, name, silver, copper);
            }
            else
            {
                info = Acore::StringFormat(SOLD_FOR_SILVER, name, silver);
            }
        }
        else
        {
            if (copper > 0 && silver > 0)
            {
                info = Acore::StringFormat(SOLD_FOR_GOLD_SILVER_COPPER, name, gold, silver, copper);
            }
            else if (copper > 0)
            {
                info = Acore::StringFormat(SOLD_FOR_GOLD_COPPER, name, gold, copper);
            }
            else if (silver > 0)
            {
                info = Acore::StringFormat(SOLD_FOR_GOLD_SILVER, name, gold, silver);
            }
            else
            {
                info = Acore::StringFormat(SOLD_FOR_GOLD, name, gold);
            }
        }

        ChatHandler(player->GetSession()).SendSysMessage(info);
    }
};

void Addmod_junk_to_gold_plusScripts()
{
    new JunkToGoldPlus();
}
