#include "framework.h"
#include "tr4_element_puzzle.h"
#include "level.h"
#include "control/control.h"
#include "Sound\sound.h"
#include "animation.h"
#include <lara.h>
#include <sphere.h>
#include <effects\effects.h>
#include <effects\tomb4fx.h>
#include <switch.h>
#include <input.h>

void HammerControl(short itemNumber)
{
    /*ITEM_INFO* item = &g_Level.Items[itemNumber];

    item->itemFlags[3] = 150;

    if (!TriggerActive(item))
    {
        *item->itemFlags = 0;
    }

    int frameNumber = item->frameNumber - g_Level.Anims[item->animNumber].frameBase;
   
    if (!item->triggerFlags)
    {
        *item->itemFlags = (frameNumber >= 52 ? 0 : 0xE0);
        AnimateItem((int)item);
    }
    else if (item->currentAnimState != 1 || item->goalAnimState != 1)
    {
        item->goalAnimState = 1;
        *item->itemFlags = (frameNumber >= 52 ? 0 : 0x7E0);

        if (frameNumber == 8)
        {
            if (item->triggerFlags == 2)
            {
                ROOM_INFO* room = &g_Level.Rooms[item->roomNumber];

                for (short linknum = room->itemNumber; linknum != NO_ITEM; linknum = g_Level.Items[linknum].nextItem)
                {
                    ITEM_INFO* currentItem = &g_Level.Items[linknum];

                    if (currentItem->objectNumber==ID_OBELISK 
                        && )
                }

                linkNum = *(signed __int16*)(Rooms + 148 * (signed __int16)item->roomNumber + 72);
                if (linkNum != -1)
                {
                    while (1)
                    {
                        v16 = 2811 * linkNum;
                        v17 = *(_WORD*)(Items + 2 * v16 + 12) == 165;
                        currentItem = (ITEM_INFO_OK*)(Items + 2 * v16);
                        if (v17
                            && currentItem->pos.yRot == -16384
                            && *(_WORD*)(Items + 5622 * (signed __int16)currentItem->itemFlags[0] + 78) == 0x4000
                            && !*(_WORD*)(Items + 5622 * (signed __int16)currentItem->itemFlags[1] + 78))
                        {
                            break;
                        }
                        linkNum = (signed __int16)currentItem->nextItem;
                        if (linkNum == -1)
                        {
                            goto LABEL_40;
                        }
                    }
                    v19 = (signed __int16)currentItem->itemFlags[0];
                    HIBYTE(currentItem->flags) |= 0x3Eu;
                    *(_BYTE*)(Items + 5622 * v19 + 41) |= 0x3Eu;
                    *(_BYTE*)(Items + 5622 * (signed __int16)currentItem->itemFlags[1] + 41) |= 0x3Eu;
                }
            LABEL_40:
                SoundEffect(255, (#64 *) & item->pos, 0);
                SoundEffect(106, (#64 *) & item->pos, 0);
                return AnimateItem((int)item);
            }
            v8 = *(signed __int16*)(Rooms + 148 * (signed __int16)item->roomNumber + 72);
            v20 = Rooms + 148 * (signed __int16)item->roomNumber;
            if (v8 != -1)
            {
                v9 = Items;
                do
                {
                    v10 = *(_WORD*)(5622 * v8 + v9 + 12);
                    v11 = 5622 * v8 + v9;
                    if (v10 >= 156
                        && v10 <= 159
                        && *(_DWORD*)(v11 + 64) == item->pos.xPos
                        && *(_DWORD*)(v11 + 72) == item->pos.zPos)
                    {
                        ExplodeItemNode(5622 * v8 + v9, 0, 0, 128);
                        KillItem(v8);
                        v9 = Items;
                        v5 = 1;
                    }
                    v8 = *(signed __int16*)(5622 * v8 + v9 + 26);
                } while (v8 != -1);
                if (v5)
                {
                    v12 = *(signed __int16*)(v20 + 72);
                    if (v12 != -1)
                    {
                        do
                        {
                            currentItem_1 = (ITEM_INFO_OK*)(5622 * v12);
                            v14 = *(WORD*)((char*)&currentItem_1->objectNumber + v9);
                            if ((v14 == 193 || v14 == 194 || v14 == 179)
                                && *(_DWORD*)((char*)&currentItem_1->pos.xPos + v9) == item->pos.xPos
                                && *(_DWORD*)((char*)&currentItem_1->pos.zPos + v9) == item->pos.zPos)
                            {
                                *(DWORD*)((char*)&currentItem_1->MainFlags + v9) &= 0xFFFFFFF9;
                                v9 = Items;
                            }
                            v12 = *(signed __int16*)((char*)&currentItem_1->nextItem + v9);
                        } while (v12 != -1);
                        return AnimateItem((int)item);
                    }
                }
            }
        }
        else if (v2 > 52 && item->triggerFlags == 2)
        {
            HIBYTE(item->flags) &= 0xC1u;
            return AnimateItem((int)item);
        }
        return AnimateItem((int)item);
    }
    v6 = item->itemFlags[2];
    if (v6)
    {
        if (v4 == 3)
        {
            HIBYTE(item->flags) &= 0xC1u;
        LABEL_8:
            item->itemFlags[2] = 0;
            return AnimateItem((int)item);
        }
        if (v4 != 4)
        {
            goto LABEL_8;
        }
        item->itemFlags[2] = v6 - 1;
        result = AnimateItem((int)item);
    }
    else
    {
        v7 = Objects[138].animIndex + 1;
        item->animNumber = Objects[138].animIndex + 1;
        item->frameNumber = *(_WORD*)(Anims + 40 * v7 + 24);
        item->goalAnimState = 2;
        item->currentAnimState = 2;
        item->itemFlags[2] = 60;
        result = AnimateItem((int)item);
    }
    return result;*/
}