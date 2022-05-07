#!/bin/bash
### Generator for std::unordered_map<std::string, std::function<void()>> VoteMsgParser
## Opreations
# reward 0~19 -> Mail Menu
for i in $(seq 0 19)
do echo "{\"reward${i}\", [this, ClientID](){
    Server()->RemMail(ClientID, ${i});
    ResetVotes(ClientID, MAILMENU);
    }},"
done
# menu 1~22 -> Go to target menu
for i in $(seq 1 22)
do echo "{\"menu${i}\", [this, ClientID](){
    ResetVotes(ClientID, ${i});
    return;
    }},"
done
# its 1~6 -> Inventory Menu
for i in $(seq 1 6)
do echo "{\"its${i}\", [this, ClientID]() {
    m_apPlayers[ClientID]->m_SelectItemType = ${i};
    ResetVotes(ClientID, INVENTORY);
    }},"
done
# scr 1~6 -> Craft Menu
for i in $(seq 1 6)
do echo "{\"scr${i}\", [this, ClientID]() {
    m_apPlayers[ClientID]->m_SortedSelectCraft = ${i};
	ResetVotes(ClientID, CRAFTING);
	return;
    }},"
done
# armor 1~3 -> Select Armor
for i in $(seq 1 3)
do echo "{\"armor${i}\", [this, ClientID]() {
    m_apPlayers[ClientID]->m_SelectArmor = ${i};
	ResetVotes(ClientID, ARMORMENU);
    return;	
    }},"
done
# sort 1~8 -> Go to target sort
for i in $(seq 1 8)
do echo "{\"sort${i}\", [this, ClientID]() {
    m_apPlayers[ClientID]->m_SortedSelectTop = ${i};
	ResetVotes(ClientID, TOPMENU);
	return;	
    }},"
done
## Items 0 ~ MAX_ITEMS - 1 (131 now)
# set -> Item Settings
# bon -> Buy Items
# seli -> Sell Items
# it -> Select Items
# cra -> Create Items
for i in $(seq 0 131)
do echo "{\"set${i}\", [this, ClientID]() {
    Server()->SetItemSettings(ClientID, ${i}, Server()->GetItemType(ClientID, ${i}));	
    ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

    m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

    if(${i} == PIZDAMET)
    {	
    	if(Server()->GetItemSettings(ClientID, PIZDAMET))
    		Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
    	else
    		Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
    }

    if(${i} == LAMPHAMMER)
    {	
    	if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
    		Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
    	else
    		Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
    }
    return;
    }},"
echo "{\"bon${i}\", [this, ClientID]() {
    BuyItem(${i}, ClientID);
    return;
    }},"
echo "{\"seli${i}\", [this, ClientID, pReason]() {
    Server()->RemItem(ClientID, ${i}, chartoint(pReason, MAX_COUNT), USEDSELL);
    return;	
    }},"
echo "{\"it${i}\", [this, ClientID]() {
    m_apPlayers[ClientID]->m_SelectItem = ${i};
    ResetVotes(ClientID, SELITEM);
    return;	
    }},"
echo "{\"cra${i}\", [this, ClientID, pReason]() {
    CreateItem(ClientID, ${i}, chartoint(pReason, MAX_COUNT));
    return;	
    }},"
done
## cs 0~63 -> Select Player
for i in $(seq 0 63)
do echo "{\"cs${i}\", [this, ClientID]() {
    str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, ${i}), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
    ResetVotes(ClientID, CSETTING);
    return;
    }},"
done