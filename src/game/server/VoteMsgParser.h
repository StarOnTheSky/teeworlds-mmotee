static std::unordered_map<std::string, std::function<void()>> VoteMsgParser = {
    {"null", [this]()
     { return; }},
    {"info", [this, pReason, ClientID]()
     {
         const char *pLanguage = m_apPlayers[ClientID]->GetLanguage();

         dynamic_string Buffer;
         Server()->Localization()->Format_L(Buffer, pLanguage, _("All owners InfClass"), NULL);
         Buffer.append("\n\n");
         Server()->Localization()->Format_L(Buffer, pLanguage, _("主要制作人员:\nNajvlad, Rem1x, Matodor, Kurosio - 原作者"), NULL);
         Buffer.append("\n\n");
         Server()->Localization()->Format_L(Buffer, pLanguage, _("天际 - 修改地图\nNinecloud - 协助开发\n天上的星星 - 开发者,服务器管理"), NULL);
         Buffer.append("\n\n");

         SendMOTD(ClientID, Buffer.buffer());
         return;
     }},
    {"rules", [this, ClientID]()
     {
         SendChatTarget(ClientID, "------- {Rules} -------");
         SendChatTarget(ClientID, "禁止使用游戏漏洞!");
         SendChatTarget(ClientID, "禁止使用辅助性软件!");
         SendChatTarget(ClientID, "禁止使用分身!");
         SendChatTarget(ClientID, "禁止同时在多个服务器登录账号!");
         SendChatTarget(ClientID, "禁止分享账号!");
         SendChatTarget(ClientID, "禁止侮辱玩家!");
         SendChatTarget(ClientID, "禁止进行线下交易!");
         SendChatTarget(ClientID, "最好不要干涉别人的游戏过程!");
         return;
     }},
    {"help", [this, ClientID]()
     {
         const char *pLanguage = m_apPlayers[ClientID]->GetLanguage();

         dynamic_string Buffer;

         Server()->Localization()->Format_L(Buffer, pLanguage, _("你需要登录(/login)或者注册(/register)一个新的账户."), NULL);
         Buffer.append("\n\n");
         Server()->Localization()->Format_L(Buffer, pLanguage, _("/register <用户名> <密码> - 创建一个新的账户"), NULL);
         Buffer.append("\n");
         Server()->Localization()->Format_L(Buffer, pLanguage, _("/login <用户名> <密码> - 登录账户"), NULL);
         Buffer.append("\n");
         // 鬼知道这个玩意是谁写的,翻译之前居然不检查一下
         // Server()->Localization()->Format_L(Buffer, pLanguage, _("/logout - 注销该账户"), NULL);
         // Buffer.append("\n\n");

         SendMOTD(ClientID, Buffer.buffer());
         return;
     }},
    {"back", [this, pReason, ClientID]()
     {
         ResetVotes(ClientID, m_apPlayers[ClientID]->m_LastVotelist);
         return;
     }},
    {"chouseopen", [this, pReason, ClientID]()
     {
         if (!Server()->GetLeader(ClientID, Server()->GetClanID(ClientID)) && !Server()->GetAdmin(ClientID, Server()->GetClanID(ClientID)))
         {
             SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("# 你不是该公会的会长或者管理员"), NULL);
             return;
         }
         if (Server()->SetOpenHouse(Server()->GetOwnHouse(ClientID)))
         {
             SendChatTarget_Localization(-1, -1, _("公会 {str:name} {str:type} 了房屋!"),
                                         "name", Server()->GetClanName(Server()->GetClanID(ClientID)), "type", Server()->GetOpenHouse(Server()->GetOwnHouse(ClientID)) ? "打开" : "关闭");
         }
         ResetVotes(ClientID, CHOUSE);
         return;
     }},
    {"ckickoff", [this, pReason, ClientID]()
     {
         if (Server()->GetLeader(ClientID, Server()->GetClanID(ClientID)) || Server()->GetAdmin(ClientID, Server()->GetClanID(ClientID)))
         {
             if (str_comp_nocase(m_apPlayers[ClientID]->m_SelectPlayer, Server()->ClientName(ClientID)) == 0)
                 return;
             if (str_comp_nocase(m_apPlayers[ClientID]->m_SelectPlayer, Server()->LeaderName(Server()->GetClanID(ClientID))) == 0 || str_comp_nocase(m_apPlayers[ClientID]->m_SelectPlayer, Server()->AdminName(Server()->GetClanID(ClientID))) == 0)
             {
                 SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("# 你不能把该公会的会长或者管理员踢出公会"), NULL);
                 return;
             }
             bool Type = false;
             for (int i = 0; i < MAX_NOBOT; ++i)
             {
                 if (m_apPlayers[i])
                 {
                     if (str_comp_nocase(m_apPlayers[ClientID]->m_SelectPlayer, Server()->ClientName(i)) == 0)
                         Type = true;
                 }
             }

             if (Type)
                 SendChatClan(Server()->GetClanID(ClientID), "会长将玩家 {str:name} 踢出了公会", "name", m_apPlayers[ClientID]->m_SelectPlayer);
             else
                 SendChatClan(Server()->GetClanID(ClientID), "会长将离线玩家 {str:name} 踢出了公会", "name", m_apPlayers[ClientID]->m_SelectPlayer);

             Server()->ExitClanOff(ClientID, m_apPlayers[ClientID]->m_SelectPlayer);
             ResetVotes(ClientID, CLANLIST);
         }
         else
             SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("# 你不是该公会的会长或者管理员"), NULL);

         return;
     }},
    {"cgetleader", [this, pReason, ClientID]()
     {
         if (Server()->GetLeader(ClientID, Server()->GetClanID(ClientID)))
         {
             if (str_comp_nocase(m_apPlayers[ClientID]->m_SelectPlayer, Server()->ClientName(ClientID)) == 0)
                 return;

             bool found = false;
             for (int i = 0; i < MAX_NOBOT; ++i)
             {
                 if (m_apPlayers[i])
                 {
                     if (Server()->GetClanID(i) == Server()->GetClanID(ClientID))
                     {
                         found = true;
                         SendChatTarget_Localization(i, CHATCATEGORY_DEFAULT, _("公会新的会长:{str:name}"), "name", m_apPlayers[ClientID]->m_SelectPlayer, NULL);
                     }
                 }
             }

             if (found)
             {
                 Server()->ChangeLeader(Server()->GetClanID(ClientID), m_apPlayers[ClientID]->m_SelectPlayer);
                 ResetVotes(ClientID, CLANLIST);
             }
         }
         else
             SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("# 你不是该公会的会长"), NULL);

         return;
     }},
    {"cgetadmin", [this, pReason, ClientID]()
     {
         if (Server()->GetLeader(ClientID, Server()->GetClanID(ClientID)))
         {
             if (str_comp_nocase(m_apPlayers[ClientID]->m_SelectPlayer, Server()->ClientName(ClientID)) == 0)
                 return;

             bool found = false;
             for (int i = 0; i < MAX_NOBOT; ++i)
             {
                 if (m_apPlayers[i])
                 {
                     if (Server()->GetClanID(i) == Server()->GetClanID(ClientID))
                     {
                         found = true;
                         SendChatTarget_Localization(i, CHATCATEGORY_DEFAULT, _("公会新的管理员:{str:name}"), "name", m_apPlayers[ClientID]->m_SelectPlayer, NULL);
                     }
                 }
             }

             if (found)
             {
                 Server()->ChangeAdmin(Server()->GetClanID(ClientID), m_apPlayers[ClientID]->m_SelectPlayer);
                 ResetVotes(ClientID, CLANLIST);
             }
         }
         else
             SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("# 你不是该公会的会长"), NULL);

         return;
     }},
    {"cremadmin", [this, pReason, ClientID]()
     {
         if (Server()->GetLeader(ClientID, Server()->GetClanID(ClientID)))
         {
             if (str_comp_nocase(m_apPlayers[ClientID]->m_SelectPlayer, Server()->ClientName(ClientID)) == 0)
                 return;

             bool found = false;
             for (int i = 0; i < MAX_NOBOT; ++i)
             {
                 if (m_apPlayers[i])
                 {
                     if (Server()->GetClanID(i) == Server()->GetClanID(ClientID))
                     {
                         found = true;
                         SendChatTarget_Localization(i, CHATCATEGORY_DEFAULT, _("公会原管理员 {str:name} 被罢免了!"), "name", m_apPlayers[ClientID]->m_SelectPlayer, NULL);
                     }
                 }
             }

             if (found)
             {
                 Server()->ChangeAdmin(Server()->GetClanID(ClientID), "Nope");
                 ResetVotes(ClientID, CLANLIST);
             }
         }
         else
             SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("# 你不是该公会的会长"), NULL);

         return;
     }},
    {"cexit", [this, pReason, ClientID]()
     {
         if (Server()->GetLeader(ClientID, Server()->GetClanID(ClientID)))
         {
             SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("会长不能退出公会"), NULL);
             return;
         }
         ExitClan(ClientID);
         return;
     }},
    {"ucconut", [this, pReason, ClientID]()
     {
         if (Server()->GetClan(Clan::MaxMemberNum, Server()->GetClanID(ClientID)) >= 25)
             return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("人数已达到最大值"), NULL);

         BuyUpgradeClan(ClientID, (m_apPlayers[ClientID]->GetNeedForUpgClan(Clan::MaxMemberNum)) * 4, Clan::MaxMemberNum, "MaxNum");
         return;
     }},
    {"uchair", [this, pReason, ClientID]()
     {					if(!Server()->GetHouse(ClientID))
						return 	SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你所在的公会没有房屋!"), NULL);

					BuyUpgradeClan(ClientID, m_apPlayers[ClientID]->GetNeedForUpgClan(Clan::ChairLevel)*2, Clan::ChairLevel,"ChairHouse"); }},
    {"uspawnhouse", [this, pReason, ClientID]()
     {
         if (!Server()->GetHouse(ClientID))
             return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你所在的公会没有房屋!"), NULL);

         if (Server()->GetSpawnInClanHouse(ClientID, 0) || Server()->GetSpawnInClanHouse(ClientID, 1))
             return;

         BuyUpgradeClan(ClientID, 500000, Clan::ExpAdd, "SpawnHouse");
     }},
    {"uaddmoney", [this, pReason, ClientID]()
     {
         BuyUpgradeClan(ClientID, m_apPlayers[ClientID]->GetNeedForUpgClan(Clan::MoneyAdd), Clan::ExpAdd, "MoneyAdd");
     }},
    {"cm1", [this, pReason, ClientID]()
     {
         unsigned long int Get = 1000;
         if (pReason[0] && isdigit(pReason[0]))
             Get = atoi(pReason);

         if (m_apPlayers[ClientID]->AccData.Gold < Get)
             Get = m_apPlayers[ClientID]->AccData.Gold;

         if (Get < 1 || Get > 100000000)
             Get = 1000;

         if (m_apPlayers[ClientID]->AccData.Gold > 0)
         {
             Server()->InitClanID(Server()->GetClanID(ClientID), PLUS, "Money", Get, true);

             m_apPlayers[ClientID]->AccData.Gold -= Get;
             m_apPlayers[ClientID]->AccData.ClanAdded += Get;
             SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你向工会捐了 {int:count} 黄金"), "count", &Get, NULL);

             UpdateStats(ClientID);
             ResetVotes(ClientID, AUTH);
             Server()->GetTopClanHouse();
             return;
         }
         else
             SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你没有那么多钱,小穷光蛋"), NULL);

         return;
     }},
    {"passquest", [this, pReason, ClientID]()
     {
         switch (m_apPlayers[ClientID]->AccData.Quest)
         {
         case 1:
             if (Server()->GetItemCount(ClientID, PIGPORNO) < QUEST1)
                 return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("任务还未完成!"), NULL);
             else
             {
                 m_apPlayers[ClientID]->ExpAdd(4000);
                 m_apPlayers[ClientID]->MoneyAdd(200000);
                 m_apPlayers[ClientID]->AccData.Quest++;
                 Server()->RemItem(ClientID, 2, QUEST1, -1);
                 UpdateStats(ClientID);
             }
             break;
         case 2:
             if (Server()->GetItemCount(ClientID, PIGPORNO) < QUEST2)
                 return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("任务还未完成!"), NULL);
             else
             {
                 m_apPlayers[ClientID]->ExpAdd(4000);
                 m_apPlayers[ClientID]->MoneyAdd(250000);
                 m_apPlayers[ClientID]->AccData.Quest++;
                 Server()->RemItem(ClientID, 2, QUEST2, -1);
                 UpdateStats(ClientID);
             }
             break;

         case 3:
             if (Server()->GetItemCount(ClientID, KWAHGANDON) < QUEST3)
                 return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("任务还未完成!"), NULL);
             else
             {
                 m_apPlayers[ClientID]->ExpAdd(8000);
                 m_apPlayers[ClientID]->MoneyAdd(500000);
                 m_apPlayers[ClientID]->AccData.Quest++;
                 Server()->RemItem(ClientID, 3, QUEST3, -1);
                 UpdateStats(ClientID);
             }
             break;

         case 4:
             if (Server()->GetItemCount(ClientID, KWAHGANDON) < QUEST4)
                 return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("任务还未完成!"), NULL);
             else
             {
                 m_apPlayers[ClientID]->ExpAdd(8000);
                 m_apPlayers[ClientID]->MoneyAdd(550000);
                 m_apPlayers[ClientID]->AccData.Quest++;
                 Server()->RemItem(ClientID, 3, QUEST4, -1);
                 UpdateStats(ClientID);
             }
             break;

         case 5:
             if (Server()->GetItemCount(ClientID, KWAHGANDON) < QUEST5 || Server()->GetItemCount(ClientID, PIGPORNO) < QUEST3)
                 return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("任务还未完成!"), NULL);
             else
             {
                 m_apPlayers[ClientID]->MoneyAdd(1000000);
                 m_apPlayers[ClientID]->AccData.Quest++;
                 Server()->RemItem(ClientID, KWAHGANDON, QUEST5, -1);
                 Server()->RemItem(ClientID, PIGPORNO, QUEST5, -1);
                 SendMail(ClientID, 2, EARRINGSKWAH, 1);
                 UpdateStats(ClientID);
             }
             break;
         case 6:
             if (Server()->GetItemCount(ClientID, KWAHGANDON) < QUEST6 || Server()->GetItemCount(ClientID, FOOTKWAH) < QUEST6)
                 return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("任务还未完成!"), NULL);
             else
             {
                 m_apPlayers[ClientID]->MoneyAdd(1050000);
                 m_apPlayers[ClientID]->AccData.Quest++;
                 Server()->RemItem(ClientID, KWAHGANDON, QUEST6, -1);
                 Server()->RemItem(ClientID, FOOTKWAH, QUEST6, -1);
                 SendMail(ClientID, 2, FORMULAWEAPON, 1);
                 SendMail(ClientID, 3, TITLEQUESTS, 1);
                 UpdateStats(ClientID);
             }
             break;
         default:
             dbg_msg("sys", "Error value %d in %s:%d", m_apPlayers[ClientID]->AccData.Quest, __FILE__, __LINE__);
             break;
         }
         ResetVotes(ClientID, QUEST);

         return;
     }},
    {"uhealth", [this, pReason, ClientID]()
     {
         int Get = 1;
         if (pReason[0] && isdigit(pReason[0]))
             Get = atoi(pReason);

         if (m_apPlayers[ClientID]->AccUpgrade.Upgrade < Get)
             Get = m_apPlayers[ClientID]->AccUpgrade.Upgrade;
         // 血量的问题不修复

         int GetSize = 0;
         switch (m_apPlayers[ClientID]->GetClass())
         {
         case PLAYERCLASS_ASSASINS:
             GetSize = AMAXHEALTH - m_apPlayers[ClientID]->AccUpgrade.Health;
             break;
         case PLAYERCLASS_BERSERK:
             GetSize = BMAXHEALTH - m_apPlayers[ClientID]->AccUpgrade.Health;
             break;
         case PLAYERCLASS_HEALER:
             GetSize = HMAXHEALTH - m_apPlayers[ClientID]->AccUpgrade.Health;
             break;
         }

         if (Get > GetSize)
             Get = GetSize;

         if (Get < 1 || Get > 1000)
             Get = 1;

         if (m_apPlayers[ClientID]->AccUpgrade.Upgrade <= 0)
             return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你没有足够的升级点"), NULL);

         if ((m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK && m_apPlayers[ClientID]->AccUpgrade.Health >= BMAXHEALTH + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)) ||
             (m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER && m_apPlayers[ClientID]->AccUpgrade.Health >= HMAXHEALTH + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)) ||
             (m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS && m_apPlayers[ClientID]->AccUpgrade.Health >= AMAXHEALTH + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)))
             return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("技能已满级"), NULL);

         m_apPlayers[ClientID]->AccUpgrade.Health += Get;
         m_apPlayers[ClientID]->AccUpgrade.Upgrade -= Get;
         SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你的技能成功提升 {int:lv} 级"), "lv", &Get, NULL);

         if (m_apPlayers[ClientID]->GetCharacter())
         {
             if (m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER)
                 m_apPlayers[ClientID]->m_HealthStart += Get * 50;
             else if (m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK)
                 m_apPlayers[ClientID]->m_HealthStart += Get * 40;
             else if (m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS)
                 m_apPlayers[ClientID]->m_HealthStart += Get * 40;
         }

         UpdateUpgrades(ClientID);
         ResetVotes(ClientID, CLMENU);
         return;
     }},
    {"udamage", [this, pReason, ClientID]()
     {
         int Get = 1;
         if (pReason[0] && isdigit(pReason[0]))
             Get = atoi(pReason);

         if (m_apPlayers[ClientID]->AccUpgrade.Upgrade < Get)
             Get = m_apPlayers[ClientID]->AccUpgrade.Upgrade;

         int GetSize = 0;
         switch (m_apPlayers[ClientID]->GetClass())
         {
         case PLAYERCLASS_ASSASINS:
             GetSize = AMAXDAMAGE - m_apPlayers[ClientID]->AccUpgrade.Damage;
             break;
         case PLAYERCLASS_BERSERK:
             GetSize = BMAXDAMAGE - m_apPlayers[ClientID]->AccUpgrade.Damage;
             break;
         case PLAYERCLASS_HEALER:
             GetSize = HMAXDAMAGE - m_apPlayers[ClientID]->AccUpgrade.Damage;
             break;
         }
         if (Get > GetSize)
             Get = GetSize;

         if (Get < 1 || Get > 1000)
             Get = 1;

         if (m_apPlayers[ClientID]->AccUpgrade.Upgrade <= 0)
             return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你没有足够的升级点"), NULL);

         if ((m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK && m_apPlayers[ClientID]->AccUpgrade.Damage >= BMAXDAMAGE + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)) ||
             (m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER && m_apPlayers[ClientID]->AccUpgrade.Damage >= HMAXDAMAGE + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)) ||
             (m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS && m_apPlayers[ClientID]->AccUpgrade.Damage >= AMAXDAMAGE + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)))
             return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("技能已满级"), NULL);

         m_apPlayers[ClientID]->AccUpgrade.Damage += Get;
         m_apPlayers[ClientID]->AccUpgrade.Upgrade -= Get;
         SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你的技能成功提升 {int:lv} 级"), "lv", &Get, NULL);

         if (((m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK && m_apPlayers[ClientID]->AccUpgrade.Damage > BMAXDAMAGE - 1) ||
              (m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER && m_apPlayers[ClientID]->AccUpgrade.Damage > HMAXDAMAGE - 1) ||
              (m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS && m_apPlayers[ClientID]->AccUpgrade.Damage > AMAXDAMAGE - 1)) &&
             Server()->GetItemCount(ClientID, SNAPDAMAGE) == 0)
         {
             // SendMail(ClientID, 4, SNAPDAMAGE, 1);
             GiveItem(ClientID, SNAPDAMAGE, 1);
             SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, ("恭喜你成功升到满级,这是你的奖励!"), NULL);
         }

         UpdateUpgrades(ClientID);
         ResetVotes(ClientID, CLMENU);

         return;
     }},
    {"uammo", [this, pReason, ClientID]()
     {				if(m_apPlayers[ClientID]->AccUpgrade.Upgrade < 5)
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你没有足够的升级点"), NULL);	
					
					if((m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK && m_apPlayers[ClientID]->AccUpgrade.Ammo >= BMAXAMMO + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER && m_apPlayers[ClientID]->AccUpgrade.Ammo >= HMAXAMMO + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS && m_apPlayers[ClientID]->AccUpgrade.Ammo >= AMAXAMMO + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)))
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("技能已满级"), NULL);
									
					m_apPlayers[ClientID]->AccUpgrade.Ammo++;
					m_apPlayers[ClientID]->AccUpgrade.Upgrade -= 5;
					SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你成功地获得了升级"), NULL);
					
					int geta = (int)(5+m_apPlayers[ClientID]->AccUpgrade.Ammo);
					Server()->SetMaxAmmo(ClientID, INFWEAPON_GUN, geta);
					Server()->SetMaxAmmo(ClientID, INFWEAPON_SHOTGUN, geta);
					Server()->SetMaxAmmo(ClientID, INFWEAPON_GRENADE, geta);
					Server()->SetMaxAmmo(ClientID, INFWEAPON_RIFLE, geta);
					
					UpdateUpgrades(ClientID);
					ResetVotes(ClientID, CLMENU);
					
					return; }},
    {"uammoregen", [this, pReason, ClientID]()
     {					int Get = 1;
					if (pReason[0] && isdigit(pReason[0]))
						Get = std::stoi(pReason);
						
					if(m_apPlayers[ClientID]->AccUpgrade.Upgrade < Get)				
						Get = m_apPlayers[ClientID]->AccUpgrade.Upgrade;

					int GetSize = 0;
					switch(m_apPlayers[ClientID]->GetClass())
					{
						case PLAYERCLASS_ASSASINS: GetSize = AMAXAREGEN-m_apPlayers[ClientID]->AccUpgrade.AmmoRegen; break;
						case PLAYERCLASS_BERSERK: GetSize = BMAXAREGEN-m_apPlayers[ClientID]->AccUpgrade.AmmoRegen; break;
						case PLAYERCLASS_HEALER: GetSize = HMAXAREGEN-m_apPlayers[ClientID]->AccUpgrade.AmmoRegen; break;
					}
					if(Get > GetSize)
						Get = GetSize;

					if(Get < 1 || Get > 1000)
						Get = 1;

					if(m_apPlayers[ClientID]->AccUpgrade.Upgrade <= 0)
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你没有足够的升级点"), NULL);	
					
					if((m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK && m_apPlayers[ClientID]->AccUpgrade.AmmoRegen >= BMAXAREGEN + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER && m_apPlayers[ClientID]->AccUpgrade.AmmoRegen >= HMAXAREGEN + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS && m_apPlayers[ClientID]->AccUpgrade.AmmoRegen >= AMAXAREGEN + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)))
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("技能已满级"), NULL);	
									
					m_apPlayers[ClientID]->AccUpgrade.AmmoRegen += Get;
					m_apPlayers[ClientID]->AccUpgrade.Upgrade -= Get;
					SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你的技能成功提升 {int:lv} 级"), "lv", &Get, NULL);							
					
					if(((m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK && m_apPlayers[ClientID]->AccUpgrade.AmmoRegen > BMAXAREGEN-1)||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER && m_apPlayers[ClientID]->AccUpgrade.AmmoRegen > HMAXAREGEN-1)||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS && m_apPlayers[ClientID]->AccUpgrade.AmmoRegen > AMAXAREGEN-1)) &&
						Server()->GetItemCount(ClientID, SNAPAMMOREGEN) == 0)
					{
						//SendMail(ClientID, 4, SNAPAMMOREGEN, 1);
						GiveItem(ClientID, SNAPAMMOREGEN, 1);
						SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT,  ("恭喜你成功升到满级,这是你的奖励!"), NULL);
					}
					
					int getar = (int)(650-m_apPlayers[ClientID]->AccUpgrade.AmmoRegen*2);
					Server()->SetAmmoRegenTime(ClientID, INFWEAPON_GRENADE, getar);
					Server()->SetAmmoRegenTime(ClientID, INFWEAPON_SHOTGUN, getar);
					Server()->SetAmmoRegenTime(ClientID, INFWEAPON_RIFLE, getar);
					
					UpdateUpgrades(ClientID);
					ResetVotes(ClientID, CLMENU);	
					return; }},
    {"uhandle", [this, pReason, ClientID]()
     {					int Get = 1;
					if (pReason[0] && isdigit(pReason[0]))
						Get = atoi (pReason);
										
					if(m_apPlayers[ClientID]->AccUpgrade.Upgrade < Get)				
						Get = m_apPlayers[ClientID]->AccUpgrade.Upgrade;

					int GetSize = 0;
					switch(m_apPlayers[ClientID]->GetClass())
					{
						case PLAYERCLASS_ASSASINS: GetSize = AMAXHANDLE-m_apPlayers[ClientID]->AccUpgrade.Speed; break;
						case PLAYERCLASS_BERSERK: GetSize = BMAXHANDLE-m_apPlayers[ClientID]->AccUpgrade.Speed; break;
						case PLAYERCLASS_HEALER: GetSize = HMAXHANDLE-m_apPlayers[ClientID]->AccUpgrade.Speed; break;
					}
					if(Get > GetSize)
						Get = GetSize;

					if(Get < 1 || Get > 1000)
						Get = 1;

					if(m_apPlayers[ClientID]->AccUpgrade.Upgrade <= 0)
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你没有足够的升级点"), NULL);	
					
					if((m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK && m_apPlayers[ClientID]->AccUpgrade.Speed >= BMAXHANDLE) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER && m_apPlayers[ClientID]->AccUpgrade.Speed >= HMAXHANDLE) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS && m_apPlayers[ClientID]->AccUpgrade.Speed >= AMAXHANDLE))
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("技能已满级"), NULL);	
					
					m_apPlayers[ClientID]->AccUpgrade.Speed += Get;
					m_apPlayers[ClientID]->AccUpgrade.Upgrade -= Get;
					SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你的技能成功提升 {int:lv} 级"), "lv", &Get, NULL);							
					
					if(((m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK && m_apPlayers[ClientID]->AccUpgrade.Speed > BMAXHANDLE-1)||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER && m_apPlayers[ClientID]->AccUpgrade.Speed > HMAXHANDLE-1)||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS && m_apPlayers[ClientID]->AccUpgrade.Speed > AMAXHANDLE-1)) &&
						Server()->GetItemCount(ClientID, SNAPHANDLE) == 0)
					{
						//SendMail(ClientID, 4, SNAPHANDLE, 1);
						GiveItem(ClientID, SNAPHANDLE, 1);
						SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT,  ("恭喜你成功升到满级,这是你的奖励!"), NULL);
					}
					
					int getsp = (int)(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*30);
					int getspg = (int)(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12);
					Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, getsp);	
					Server()->SetFireDelay(ClientID, INFWEAPON_GUN, getsp);
					Server()->SetFireDelay(ClientID, INFWEAPON_SHOTGUN, getsp);
					Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, getspg);
					Server()->SetFireDelay(ClientID, INFWEAPON_RIFLE, getsp);
					
					UpdateUpgrades(ClientID);
					ResetVotes(ClientID, CLMENU);	
					return; }},
    {"umana", [this, pReason, ClientID]()
     {					int Get = 1;
					if (pReason[0] && isdigit(pReason[0]))
						Get = atoi (pReason);
										
					if(m_apPlayers[ClientID]->AccUpgrade.Upgrade < Get)				
						Get = m_apPlayers[ClientID]->AccUpgrade.Upgrade;

					if(Get < 1 || Get > 1000)
						Get = 1;

					if(m_apPlayers[ClientID]->AccUpgrade.Upgrade <= 0)
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你没有足够的升级点"), NULL);	
					
					m_apPlayers[ClientID]->AccUpgrade.Mana += Get;
					m_apPlayers[ClientID]->AccUpgrade.Upgrade -= Get;
					SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你成功地升了 {int:lv} 级"), "lv", &Get, NULL);							
	
					UpdateUpgrades(ClientID);
					ResetVotes(ClientID, CLMENU);	
					return; }},
    {"uspray", [this, pReason, ClientID]()
     {					if(m_apPlayers[ClientID]->AccUpgrade.Upgrade < 10)
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你没有足够的升级点"), NULL);	
					
					if((m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK && m_apPlayers[ClientID]->AccUpgrade.Spray >= BMAXSPREAD) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER && m_apPlayers[ClientID]->AccUpgrade.Spray >= HMAXSPREAD) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS && m_apPlayers[ClientID]->AccUpgrade.Spray >= AMAXSPREAD))
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("技能已满级"), NULL);	
					
					m_apPlayers[ClientID]->AccUpgrade.Spray++;
					m_apPlayers[ClientID]->AccUpgrade.Upgrade -= 10;
					SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你的技能成功提升 {int:lv} 级"), NULL);							
					
					UpdateUpgrades(ClientID);
					ResetVotes(ClientID, CLMENU);	
					
					return; }},
    {"uhpregen", [this, pReason, ClientID]()
     {					int Get = 1;
					if (pReason[0] && isdigit(pReason[0]))
						Get = atoi (pReason);

					if(m_apPlayers[ClientID]->AccUpgrade.Upgrade < Get)				
						Get = m_apPlayers[ClientID]->AccUpgrade.Upgrade;

					int GetSize = 0;
					switch(m_apPlayers[ClientID]->GetClass())
					{
						case PLAYERCLASS_ASSASINS: GetSize = AMAXHPREGEN-m_apPlayers[ClientID]->AccUpgrade.HPRegen; break;
						case PLAYERCLASS_BERSERK: GetSize = BMAXHPREGEN-m_apPlayers[ClientID]->AccUpgrade.HPRegen; break;
						case PLAYERCLASS_HEALER: GetSize = HMAXHPREGEN-m_apPlayers[ClientID]->AccUpgrade.HPRegen; break;
					}
					if(Get > GetSize)
						Get = GetSize;
					
					if(Get < 1 || Get > 1000)
						Get = 1;
					
					if(m_apPlayers[ClientID]->AccUpgrade.Upgrade <= 0)
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你没有足够的升级点"), NULL);	
					
					if((m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK && m_apPlayers[ClientID]->AccUpgrade.HPRegen >= BMAXHPREGEN + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER && m_apPlayers[ClientID]->AccUpgrade.HPRegen >= HMAXHPREGEN + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS && m_apPlayers[ClientID]->AccUpgrade.HPRegen >= AMAXHPREGEN + 3 * Server()->GetItemCount(ClientID, EXTENDLIMIT)))
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("技能已满级"), NULL);	
							
					m_apPlayers[ClientID]->AccUpgrade.HPRegen += Get;
					m_apPlayers[ClientID]->AccUpgrade.Upgrade -= Get;
					SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你的技能成功提升 {int:lv} 级"), "lv", &Get, NULL);							
					
					UpdateUpgrades(ClientID);	
					ResetVotes(ClientID, CLMENU);	
					return; }},
    {"ushammerrange", [this, pReason, ClientID]()
     {					if(m_apPlayers[ClientID]->AccUpgrade.SkillPoint < HAMMERRANGE)
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你没有足够的技能点"), NULL);	
					
					if((m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK && m_apPlayers[ClientID]->AccUpgrade.HammerRange > 4) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER && m_apPlayers[ClientID]->AccUpgrade.HammerRange > 5) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS && m_apPlayers[ClientID]->AccUpgrade.HammerRange > 7))
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("技能已满级"), NULL);	

					m_apPlayers[ClientID]->AccUpgrade.HammerRange++;
					m_apPlayers[ClientID]->AccUpgrade.SkillPoint -= HAMMERRANGE;
					SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你的技能成功地提升了"), NULL);							
						
					UpdateUpgrades(ClientID);
					ResetVotes(ClientID, CLMENU);	
					return; }},
    {"upasive2", [this, pReason, ClientID]()
     {if(m_apPlayers[ClientID]->AccUpgrade.SkillPoint < HAMMERRANGE)
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你没有足够的技能点"), NULL);	
					
					if((m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_BERSERK && m_apPlayers[ClientID]->AccUpgrade.Pasive2 > 4) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_HEALER && m_apPlayers[ClientID]->AccUpgrade.Pasive2 > 10) ||
						(m_apPlayers[ClientID]->GetClass() == PLAYERCLASS_ASSASINS && m_apPlayers[ClientID]->AccUpgrade.Pasive2 > 7))
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("技能已满级"), NULL);	
					
					m_apPlayers[ClientID]->AccUpgrade.Pasive2++;
					m_apPlayers[ClientID]->AccUpgrade.SkillPoint -= HAMMERRANGE;
					SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("你的技能成功地提升了"), NULL);							
						
					UpdateUpgrades(ClientID);
						
					ResetVotes(ClientID, CLMENU);	
					   
					return; }},
    {"uskillwall", [this, pReason, ClientID]()
     {
         BuySkill(ClientID, 70, SKWALL);
         return;
     }},
    {"uskillsword", [this, pReason, ClientID]()
     {					BuySkill(ClientID, 20, SSWORD);
					return; }},
    {"uskillheal", [this, pReason, ClientID]()
     {					BuySkill(ClientID, 60, SKHEAL);
					return; }},
    {"bvip", [this, pReason, ClientID]()
     {					Server()->SetItemPrice(ClientID, VIPPACKAGE, 0, 1000);
					BuyItem(VIPPACKAGE, ClientID, 1);
					return; }},
    {"bsp", [this, pReason, ClientID]()
     {					Server()->SetItemPrice(ClientID, SKILLUPBOX, 0, 200);
					BuyItem(SKILLUPBOX, ClientID, 1);
					return; }},
    {"bantipvp", [this, pReason, ClientID]()
     {					Server()->SetItemPrice(ClientID, SANTIPVP, 0, 200);
					BuyItem(SANTIPVP, ClientID, 1);
					return; }},
    {"ssantiping", [this, pReason, ClientID]()
     {					if(Server()->GetClientAntiPing(ClientID))
						Server()->SetClientAntiPing(ClientID, 0);
					else 
						Server()->SetClientAntiPing(ClientID, 1);
						
					ResetVotes(ClientID, SETTINGS);	
					return; }},
    {"sssecurity", [this, pReason, ClientID]()
     {					if(Server()->GetSecurity(ClientID))
						Server()->SetSecurity(ClientID, 0);
					else 
						Server()->SetSecurity(ClientID, 1);
						
					ResetVotes(ClientID, SETTINGS);	
					return; }},
    {"sssettingschat", [this, pReason, ClientID]()
     {					int Get = Server()->GetItemSettings(ClientID, SCHAT)+1;
					if(Get > 2) Server()->SetItemSettingsCount(ClientID, SCHAT, 0);
					else Server()->SetItemSettingsCount(ClientID, SCHAT, Get);
						
					UpdateStats(ClientID);
					ResetVotes(ClientID, SETTINGS);	
					return; }},
    {"sskillwall", [this, pReason, ClientID]()
     {					int Get = Server()->GetItemSettings(ClientID, SKWALL)+1;
					if(Get > 7) Server()->SetItemSettingsCount(ClientID, SKWALL, 0);
					else Server()->SetItemSettingsCount(ClientID, SKWALL, Get);
						
					UpdateStats(ClientID);
					ResetVotes(ClientID, CLMENU);	
					return; }},
    {"sskillheal", [this, pReason, ClientID]()
     {					int Get = Server()->GetItemSettings(ClientID, SKHEAL)+1;
					if(Get > 7) Server()->SetItemSettingsCount(ClientID, SKHEAL, 0);
					else Server()->SetItemSettingsCount(ClientID, SKHEAL, Get);
						
					UpdateStats(ClientID);
					ResetVotes(ClientID, CLMENU);	
					return; }},
    {"sskillsword", [this, pReason, ClientID]()
     {					int Get = Server()->GetItemSettings(ClientID, SSWORD)+1;
					if(Get > 7) Server()->SetItemSettingsCount(ClientID, SSWORD, 0);
					else Server()->SetItemSettingsCount(ClientID, SSWORD, Get);
						
					UpdateStats(ClientID);
					ResetVotes(ClientID, CLMENU);	
					return; }},
    {"sskillsummer", [this, pReason, ClientID]()
     {					int Get = Server()->GetItemSettings(ClientID, SHEALSUMMER)+1;
					if(Get > 7) Server()->SetItemSettingsCount(ClientID, SHEALSUMMER, 0);
					else Server()->SetItemSettingsCount(ClientID, SHEALSUMMER, Get);
						
					UpdateStats(ClientID);
					ResetVotes(ClientID, CLMENU);	
					return; }},
    {"semote", [this, pReason, ClientID]()
     {
						int Get = Server()->GetItemSettings(ClientID, MODULEEMOTE)+1;
					if(Get > 5) Server()->SetItemSettingsCount(ClientID, MODULEEMOTE, 0);
					else Server()->SetItemSettingsCount(ClientID, MODULEEMOTE, Get);
						
					UpdateStats(ClientID);
					ResetVotes(ClientID, SETTINGS);	
					return; }},
    {"test", [this, pReason, ClientID]()
     {
						dbg_msg("test", "Size: %lf, Count: %lf", VoteMsgParser.size(), VoteMsgParser.count("test")); return; }},
    {"ssantipvp", [this, pReason, ClientID]()
     {
						Server()->SetItemSettings(ClientID, SANTIPVP);
					ResetVotes(ClientID, SETTINGS);	
					return; }},
    {"ssetingsdrop", [this, pReason, ClientID]()
     {
         Server()->SetItemSettings(ClientID, SDROP);

         UpdateStats(ClientID);
         ResetVotes(ClientID, SETTINGS);
         return;
     }},
    {"useitem", [this, pReason, ClientID]()
     {
         if (m_apPlayers[ClientID]->m_LastChangeInfo && m_apPlayers[ClientID]->m_LastChangeInfo + Server()->TickSpeed() * 3 > Server()->Tick())
             return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("请等待..."), NULL);

         m_apPlayers[ClientID]->m_LastChangeInfo = Server()->Tick();
         int SelectItem = m_apPlayers[ClientID]->m_SelectItem;

         int Get = chartoint(pReason, MAX_COUNT);
         if (SelectItem == RESETINGUPGRADE || SelectItem == RESETINGSKILL || SelectItem == VIPPACKAGE)
             Get = 1;

         Server()->RemItem(ClientID, SelectItem, Get, USEDUSE);
         m_apPlayers[ClientID]->m_SelectItem = -1;
         ResetVotes(ClientID, AUTH);
         return;
     }},
    {"sellitem", [this, pReason, ClientID]()
     {
         int SelectItem = m_apPlayers[ClientID]->m_SelectItem;

         Server()->RemItem(ClientID, SelectItem, chartoint(pReason, MAX_COUNT), USEDSELL);
         m_apPlayers[ClientID]->m_SelectItem = -1;
         return;
     }},
    {"dropitem", [this, pReason, ClientID]()
     {					if(m_apPlayers[ClientID]->m_LastChangeInfo && m_apPlayers[ClientID]->m_LastChangeInfo+Server()->TickSpeed()*3 > Server()->Tick())
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("请稍候..."), NULL);

					m_apPlayers[ClientID]->m_LastChangeInfo = Server()->Tick();
					int SelectItem = m_apPlayers[ClientID]->m_SelectItem;					
					Server()->RemItem(ClientID, SelectItem, chartoint(pReason, MAX_COUNT), USEDDROP); // Выброс предметов для всех игроков
					m_apPlayers[ClientID]->m_SelectItem = -1;
					ResetVotes(ClientID, AUTH);
					return; }},
    {"enchantitem", [this, pReason, ClientID]()
     {					if(m_apPlayers[ClientID]->m_LastChangeInfo && m_apPlayers[ClientID]->m_LastChangeInfo+Server()->TickSpeed()*3 > Server()->Tick())
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("请等待..."), NULL);

					m_apPlayers[ClientID]->m_LastChangeInfo = Server()->Tick();
					int SelectItem = m_apPlayers[ClientID]->m_SelectItem;					
					if(Server()->GetItemCount(ClientID, MATERIAL) < 1000)
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("需要 1000 材料(material)"), NULL); 	

					Server()->RemItem(ClientID, MATERIAL, 1000, -1);

					auto p = (float)(1.0f / (1 + Server()->GetItemEnchant(ClientID, SelectItem)));
					//TODO
					if(random_prob(p))
					{
						Server()->SetItemEnchant(ClientID, SelectItem, Server()->GetItemEnchant(ClientID, SelectItem)+1);

						int Enchant = Server()->GetItemEnchant(ClientID, SelectItem);
						SendChatTarget_Localization(-1, CHATCATEGORY_DEFAULT, _("{str:name} 成功地附魔了物品:{str:item} +{int:enchant}"), 
							"name", Server()->ClientName(ClientID), "item", Server()->GetItemName(ClientID, SelectItem), "enchant", &Enchant, NULL); 
					
						if(Enchant == 10 && !Server()->GetItemCount(ClientID, TITLEENCHANT))
						{
							SendMail(ClientID, 3, TITLEENCHANT, 1);
						}
					}
					else SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("升级失败."), NULL); 
				
					m_apPlayers[ClientID]->m_SelectItem = -1;
					ResetVotes(ClientID, ARMORMENU);
					return; }},
    {"destitem", [this, pReason, ClientID, pPlayer]()
     {					if(m_apPlayers[ClientID]->m_LastChangeInfo && m_apPlayers[ClientID]->m_LastChangeInfo+Server()->TickSpeed()*3 > Server()->Tick())
						return SendChatTarget_Localization(ClientID, CHATCATEGORY_DEFAULT, _("请等候..."), NULL);

					m_apPlayers[ClientID]->m_LastChangeInfo = Server()->Tick();
					int SelectItem = m_apPlayers[ClientID]->m_SelectItem;
					int Get = chartoint(pReason, MAX_COUNT);
					
					if(Server()->GetItemCount(ClientID, SelectItem) < Get)	
						Get = Server()->GetItemCount(ClientID, SelectItem);
					
					if(!Server()->GetItemCount(ClientID, SelectItem))
					{
						if(m_apPlayers[ClientID]->GetCharacter())
							switch(SelectItem)
							{
								case IGUN:	m_apPlayers[ClientID]->GetCharacter()->RemoveGun(WEAPON_GUN); break;
								case ISHOTGUN: m_apPlayers[ClientID]->GetCharacter()->RemoveGun(WEAPON_SHOTGUN); break;
								case IGRENADE: m_apPlayers[ClientID]->GetCharacter()->RemoveGun(WEAPON_GRENADE); break;
								case ILASER: m_apPlayers[ClientID]->GetCharacter()->RemoveGun(WEAPON_RIFLE); break;
							}
					}
					Server()->RemItem(ClientID, SelectItem, Get, -1);

					if(Server()->GetItemType(ClientID, SelectItem) == 15 || Server()->GetItemType(ClientID, SelectItem) == 16)
						m_pController->OnPlayerInfoChange(pPlayer);

					m_apPlayers[ClientID]->m_SelectItem = -1;
					ResetVotes(ClientID, AUTH);
					return; }},
    {"getolbonus", [this, pReason, ClientID]()
     {Server()->InitMailID(ClientID);
					m_apPlayers[ClientID]->m_LastChangeInfo = Server()->Tick();
					Server()->RemMail_OnlineBonus(ClientID);
					ResetVotes(ClientID, MAILMENU); }},
    {"reward0", [this, ClientID](){
        Server()->RemMail(ClientID, 0);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward1", [this, ClientID](){
        Server()->RemMail(ClientID, 1);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward2", [this, ClientID](){
        Server()->RemMail(ClientID, 2);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward3", [this, ClientID](){
        Server()->RemMail(ClientID, 3);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward4", [this, ClientID](){
        Server()->RemMail(ClientID, 4);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward5", [this, ClientID](){
        Server()->RemMail(ClientID, 5);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward6", [this, ClientID](){
        Server()->RemMail(ClientID, 6);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward7", [this, ClientID](){
        Server()->RemMail(ClientID, 7);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward8", [this, ClientID](){
        Server()->RemMail(ClientID, 8);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward9", [this, ClientID](){
        Server()->RemMail(ClientID, 9);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward10", [this, ClientID](){
        Server()->RemMail(ClientID, 10);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward11", [this, ClientID](){
        Server()->RemMail(ClientID, 11);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward12", [this, ClientID](){
        Server()->RemMail(ClientID, 12);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward13", [this, ClientID](){
        Server()->RemMail(ClientID, 13);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward14", [this, ClientID](){
        Server()->RemMail(ClientID, 14);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward15", [this, ClientID](){
        Server()->RemMail(ClientID, 15);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward16", [this, ClientID](){
        Server()->RemMail(ClientID, 16);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward17", [this, ClientID](){
        Server()->RemMail(ClientID, 17);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward18", [this, ClientID](){
        Server()->RemMail(ClientID, 18);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"reward19", [this, ClientID](){
        Server()->RemMail(ClientID, 19);
        ResetVotes(ClientID, MAILMENU);
        }},
    {"menu1", [this, ClientID](){
        ResetVotes(ClientID, 1);
        return;
        }},
    {"menu2", [this, ClientID](){
        ResetVotes(ClientID, 2);
        return;
        }},
    {"menu3", [this, ClientID](){
        ResetVotes(ClientID, 3);
        return;
        }},
    {"menu4", [this, ClientID](){
        ResetVotes(ClientID, 4);
        return;
        }},
    {"menu5", [this, ClientID](){
        ResetVotes(ClientID, 5);
        return;
        }},
    {"menu6", [this, ClientID](){
        ResetVotes(ClientID, 6);
        return;
        }},
    {"menu7", [this, ClientID](){
        ResetVotes(ClientID, 7);
        return;
        }},
    {"menu8", [this, ClientID](){
        ResetVotes(ClientID, 8);
        return;
        }},
    {"menu9", [this, ClientID](){
        ResetVotes(ClientID, 9);
        return;
        }},
    {"menu10", [this, ClientID](){
        ResetVotes(ClientID, 10);
        return;
        }},
    {"menu11", [this, ClientID](){
        ResetVotes(ClientID, 11);
        return;
        }},
    {"menu12", [this, ClientID](){
        ResetVotes(ClientID, 12);
        return;
        }},
    {"menu13", [this, ClientID](){
        ResetVotes(ClientID, 13);
        return;
        }},
    {"menu14", [this, ClientID](){
        ResetVotes(ClientID, 14);
        return;
        }},
    {"menu15", [this, ClientID](){
        ResetVotes(ClientID, 15);
        return;
        }},
    {"menu16", [this, ClientID](){
        ResetVotes(ClientID, 16);
        return;
        }},
    {"menu17", [this, ClientID](){
        ResetVotes(ClientID, 17);
        return;
        }},
    {"menu18", [this, ClientID](){
        ResetVotes(ClientID, 18);
        return;
        }},
    {"menu19", [this, ClientID](){
        ResetVotes(ClientID, 19);
        return;
        }},
    {"menu20", [this, ClientID](){
        ResetVotes(ClientID, 20);
        return;
        }},
    {"menu21", [this, ClientID](){
        ResetVotes(ClientID, 21);
        return;
        }},
    {"menu22", [this, ClientID](){
        ResetVotes(ClientID, 22);
        return;
        }},
    {"its1", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItemType = 1;
        ResetVotes(ClientID, INVENTORY);
        }},
    {"its2", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItemType = 2;
        ResetVotes(ClientID, INVENTORY);
        }},
    {"its3", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItemType = 3;
        ResetVotes(ClientID, INVENTORY);
        }},
    {"its4", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItemType = 4;
        ResetVotes(ClientID, INVENTORY);
        }},
    {"its5", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItemType = 5;
        ResetVotes(ClientID, INVENTORY);
        }},
    {"its6", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItemType = 6;
        ResetVotes(ClientID, INVENTORY);
        }},
    {"scr1", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectCraft = 1;
        ResetVotes(ClientID, CRAFTING);
        return;
        }},
    {"scr2", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectCraft = 2;
        ResetVotes(ClientID, CRAFTING);
        return;
        }},
    {"scr3", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectCraft = 3;
        ResetVotes(ClientID, CRAFTING);
        return;
        }},
    {"scr4", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectCraft = 4;
        ResetVotes(ClientID, CRAFTING);
        return;
        }},
    {"scr5", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectCraft = 5;
        ResetVotes(ClientID, CRAFTING);
        return;
        }},
    {"scr6", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectCraft = 6;
        ResetVotes(ClientID, CRAFTING);
        return;
        }},
    {"armor1", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectArmor = 1;
        ResetVotes(ClientID, ARMORMENU);
        return;	
        }},
    {"armor2", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectArmor = 2;
        ResetVotes(ClientID, ARMORMENU);
        return;	
        }},
    {"armor3", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectArmor = 3;
        ResetVotes(ClientID, ARMORMENU);
        return;	
        }},
    {"sort1", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectTop = 1;
        ResetVotes(ClientID, TOPMENU);
        return;	
        }},
    {"sort2", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectTop = 2;
        ResetVotes(ClientID, TOPMENU);
        return;	
        }},
    {"sort3", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectTop = 3;
        ResetVotes(ClientID, TOPMENU);
        return;	
        }},
    {"sort4", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectTop = 4;
        ResetVotes(ClientID, TOPMENU);
        return;	
        }},
    {"sort5", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectTop = 5;
        ResetVotes(ClientID, TOPMENU);
        return;	
        }},
    {"sort6", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectTop = 6;
        ResetVotes(ClientID, TOPMENU);
        return;	
        }},
    {"sort7", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectTop = 7;
        ResetVotes(ClientID, TOPMENU);
        return;	
        }},
    {"sort8", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SortedSelectTop = 8;
        ResetVotes(ClientID, TOPMENU);
        return;	
        }},
    {"set0", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 0, Server()->GetItemType(ClientID, 0));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(0 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(0 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon0", [this, ClientID]() {
        BuyItem(0, ClientID);
        return;
        }},
    {"seli0", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 0, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it0", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 0;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra0", [this, ClientID, pReason]() {
        CreateItem(ClientID, 0, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set1", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 1, Server()->GetItemType(ClientID, 1));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(1 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(1 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon1", [this, ClientID]() {
        BuyItem(1, ClientID);
        return;
        }},
    {"seli1", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 1, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it1", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 1;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra1", [this, ClientID, pReason]() {
        CreateItem(ClientID, 1, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set2", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 2, Server()->GetItemType(ClientID, 2));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(2 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(2 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon2", [this, ClientID]() {
        BuyItem(2, ClientID);
        return;
        }},
    {"seli2", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 2, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it2", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 2;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra2", [this, ClientID, pReason]() {
        CreateItem(ClientID, 2, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set3", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 3, Server()->GetItemType(ClientID, 3));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(3 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(3 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon3", [this, ClientID]() {
        BuyItem(3, ClientID);
        return;
        }},
    {"seli3", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 3, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it3", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 3;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra3", [this, ClientID, pReason]() {
        CreateItem(ClientID, 3, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set4", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 4, Server()->GetItemType(ClientID, 4));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(4 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(4 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon4", [this, ClientID]() {
        BuyItem(4, ClientID);
        return;
        }},
    {"seli4", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 4, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it4", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 4;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra4", [this, ClientID, pReason]() {
        CreateItem(ClientID, 4, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set5", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 5, Server()->GetItemType(ClientID, 5));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(5 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(5 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon5", [this, ClientID]() {
        BuyItem(5, ClientID);
        return;
        }},
    {"seli5", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 5, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it5", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 5;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra5", [this, ClientID, pReason]() {
        CreateItem(ClientID, 5, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set6", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 6, Server()->GetItemType(ClientID, 6));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(6 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(6 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon6", [this, ClientID]() {
        BuyItem(6, ClientID);
        return;
        }},
    {"seli6", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 6, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it6", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 6;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra6", [this, ClientID, pReason]() {
        CreateItem(ClientID, 6, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set7", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 7, Server()->GetItemType(ClientID, 7));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(7 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(7 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon7", [this, ClientID]() {
        BuyItem(7, ClientID);
        return;
        }},
    {"seli7", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 7, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it7", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 7;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra7", [this, ClientID, pReason]() {
        CreateItem(ClientID, 7, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set8", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 8, Server()->GetItemType(ClientID, 8));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(8 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(8 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon8", [this, ClientID]() {
        BuyItem(8, ClientID);
        return;
        }},
    {"seli8", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 8, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it8", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 8;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra8", [this, ClientID, pReason]() {
        CreateItem(ClientID, 8, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set9", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 9, Server()->GetItemType(ClientID, 9));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(9 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(9 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon9", [this, ClientID]() {
        BuyItem(9, ClientID);
        return;
        }},
    {"seli9", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 9, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it9", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 9;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra9", [this, ClientID, pReason]() {
        CreateItem(ClientID, 9, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set10", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 10, Server()->GetItemType(ClientID, 10));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(10 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(10 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon10", [this, ClientID]() {
        BuyItem(10, ClientID);
        return;
        }},
    {"seli10", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 10, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it10", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 10;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra10", [this, ClientID, pReason]() {
        CreateItem(ClientID, 10, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set11", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 11, Server()->GetItemType(ClientID, 11));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(11 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(11 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon11", [this, ClientID]() {
        BuyItem(11, ClientID);
        return;
        }},
    {"seli11", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 11, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it11", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 11;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra11", [this, ClientID, pReason]() {
        CreateItem(ClientID, 11, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set12", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 12, Server()->GetItemType(ClientID, 12));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(12 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(12 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon12", [this, ClientID]() {
        BuyItem(12, ClientID);
        return;
        }},
    {"seli12", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 12, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it12", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 12;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra12", [this, ClientID, pReason]() {
        CreateItem(ClientID, 12, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set13", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 13, Server()->GetItemType(ClientID, 13));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(13 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(13 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon13", [this, ClientID]() {
        BuyItem(13, ClientID);
        return;
        }},
    {"seli13", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 13, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it13", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 13;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra13", [this, ClientID, pReason]() {
        CreateItem(ClientID, 13, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set14", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 14, Server()->GetItemType(ClientID, 14));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(14 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(14 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon14", [this, ClientID]() {
        BuyItem(14, ClientID);
        return;
        }},
    {"seli14", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 14, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it14", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 14;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra14", [this, ClientID, pReason]() {
        CreateItem(ClientID, 14, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set15", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 15, Server()->GetItemType(ClientID, 15));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(15 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(15 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon15", [this, ClientID]() {
        BuyItem(15, ClientID);
        return;
        }},
    {"seli15", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 15, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it15", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 15;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra15", [this, ClientID, pReason]() {
        CreateItem(ClientID, 15, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set16", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 16, Server()->GetItemType(ClientID, 16));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(16 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(16 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon16", [this, ClientID]() {
        BuyItem(16, ClientID);
        return;
        }},
    {"seli16", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 16, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it16", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 16;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra16", [this, ClientID, pReason]() {
        CreateItem(ClientID, 16, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set17", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 17, Server()->GetItemType(ClientID, 17));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(17 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(17 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon17", [this, ClientID]() {
        BuyItem(17, ClientID);
        return;
        }},
    {"seli17", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 17, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it17", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 17;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra17", [this, ClientID, pReason]() {
        CreateItem(ClientID, 17, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set18", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 18, Server()->GetItemType(ClientID, 18));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(18 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(18 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon18", [this, ClientID]() {
        BuyItem(18, ClientID);
        return;
        }},
    {"seli18", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 18, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it18", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 18;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra18", [this, ClientID, pReason]() {
        CreateItem(ClientID, 18, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set19", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 19, Server()->GetItemType(ClientID, 19));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(19 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(19 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon19", [this, ClientID]() {
        BuyItem(19, ClientID);
        return;
        }},
    {"seli19", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 19, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it19", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 19;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra19", [this, ClientID, pReason]() {
        CreateItem(ClientID, 19, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set20", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 20, Server()->GetItemType(ClientID, 20));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(20 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(20 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon20", [this, ClientID]() {
        BuyItem(20, ClientID);
        return;
        }},
    {"seli20", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 20, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it20", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 20;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra20", [this, ClientID, pReason]() {
        CreateItem(ClientID, 20, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set21", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 21, Server()->GetItemType(ClientID, 21));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(21 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(21 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon21", [this, ClientID]() {
        BuyItem(21, ClientID);
        return;
        }},
    {"seli21", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 21, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it21", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 21;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra21", [this, ClientID, pReason]() {
        CreateItem(ClientID, 21, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set22", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 22, Server()->GetItemType(ClientID, 22));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(22 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(22 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon22", [this, ClientID]() {
        BuyItem(22, ClientID);
        return;
        }},
    {"seli22", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 22, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it22", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 22;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra22", [this, ClientID, pReason]() {
        CreateItem(ClientID, 22, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set23", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 23, Server()->GetItemType(ClientID, 23));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(23 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(23 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon23", [this, ClientID]() {
        BuyItem(23, ClientID);
        return;
        }},
    {"seli23", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 23, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it23", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 23;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra23", [this, ClientID, pReason]() {
        CreateItem(ClientID, 23, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set24", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 24, Server()->GetItemType(ClientID, 24));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(24 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(24 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon24", [this, ClientID]() {
        BuyItem(24, ClientID);
        return;
        }},
    {"seli24", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 24, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it24", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 24;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra24", [this, ClientID, pReason]() {
        CreateItem(ClientID, 24, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set25", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 25, Server()->GetItemType(ClientID, 25));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(25 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(25 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon25", [this, ClientID]() {
        BuyItem(25, ClientID);
        return;
        }},
    {"seli25", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 25, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it25", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 25;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra25", [this, ClientID, pReason]() {
        CreateItem(ClientID, 25, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set26", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 26, Server()->GetItemType(ClientID, 26));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(26 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(26 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon26", [this, ClientID]() {
        BuyItem(26, ClientID);
        return;
        }},
    {"seli26", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 26, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it26", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 26;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra26", [this, ClientID, pReason]() {
        CreateItem(ClientID, 26, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set27", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 27, Server()->GetItemType(ClientID, 27));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(27 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(27 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon27", [this, ClientID]() {
        BuyItem(27, ClientID);
        return;
        }},
    {"seli27", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 27, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it27", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 27;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra27", [this, ClientID, pReason]() {
        CreateItem(ClientID, 27, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set28", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 28, Server()->GetItemType(ClientID, 28));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(28 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(28 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon28", [this, ClientID]() {
        BuyItem(28, ClientID);
        return;
        }},
    {"seli28", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 28, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it28", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 28;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra28", [this, ClientID, pReason]() {
        CreateItem(ClientID, 28, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set29", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 29, Server()->GetItemType(ClientID, 29));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(29 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(29 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon29", [this, ClientID]() {
        BuyItem(29, ClientID);
        return;
        }},
    {"seli29", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 29, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it29", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 29;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra29", [this, ClientID, pReason]() {
        CreateItem(ClientID, 29, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set30", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 30, Server()->GetItemType(ClientID, 30));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(30 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(30 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon30", [this, ClientID]() {
        BuyItem(30, ClientID);
        return;
        }},
    {"seli30", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 30, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it30", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 30;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra30", [this, ClientID, pReason]() {
        CreateItem(ClientID, 30, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set31", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 31, Server()->GetItemType(ClientID, 31));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(31 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(31 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon31", [this, ClientID]() {
        BuyItem(31, ClientID);
        return;
        }},
    {"seli31", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 31, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it31", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 31;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra31", [this, ClientID, pReason]() {
        CreateItem(ClientID, 31, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set32", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 32, Server()->GetItemType(ClientID, 32));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(32 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(32 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon32", [this, ClientID]() {
        BuyItem(32, ClientID);
        return;
        }},
    {"seli32", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 32, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it32", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 32;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra32", [this, ClientID, pReason]() {
        CreateItem(ClientID, 32, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set33", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 33, Server()->GetItemType(ClientID, 33));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(33 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(33 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon33", [this, ClientID]() {
        BuyItem(33, ClientID);
        return;
        }},
    {"seli33", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 33, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it33", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 33;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra33", [this, ClientID, pReason]() {
        CreateItem(ClientID, 33, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set34", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 34, Server()->GetItemType(ClientID, 34));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(34 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(34 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon34", [this, ClientID]() {
        BuyItem(34, ClientID);
        return;
        }},
    {"seli34", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 34, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it34", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 34;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra34", [this, ClientID, pReason]() {
        CreateItem(ClientID, 34, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set35", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 35, Server()->GetItemType(ClientID, 35));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(35 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(35 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon35", [this, ClientID]() {
        BuyItem(35, ClientID);
        return;
        }},
    {"seli35", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 35, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it35", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 35;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra35", [this, ClientID, pReason]() {
        CreateItem(ClientID, 35, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set36", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 36, Server()->GetItemType(ClientID, 36));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(36 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(36 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon36", [this, ClientID]() {
        BuyItem(36, ClientID);
        return;
        }},
    {"seli36", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 36, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it36", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 36;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra36", [this, ClientID, pReason]() {
        CreateItem(ClientID, 36, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set37", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 37, Server()->GetItemType(ClientID, 37));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(37 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(37 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon37", [this, ClientID]() {
        BuyItem(37, ClientID);
        return;
        }},
    {"seli37", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 37, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it37", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 37;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra37", [this, ClientID, pReason]() {
        CreateItem(ClientID, 37, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set38", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 38, Server()->GetItemType(ClientID, 38));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(38 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(38 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon38", [this, ClientID]() {
        BuyItem(38, ClientID);
        return;
        }},
    {"seli38", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 38, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it38", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 38;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra38", [this, ClientID, pReason]() {
        CreateItem(ClientID, 38, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set39", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 39, Server()->GetItemType(ClientID, 39));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(39 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(39 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon39", [this, ClientID]() {
        BuyItem(39, ClientID);
        return;
        }},
    {"seli39", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 39, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it39", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 39;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra39", [this, ClientID, pReason]() {
        CreateItem(ClientID, 39, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set40", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 40, Server()->GetItemType(ClientID, 40));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(40 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(40 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon40", [this, ClientID]() {
        BuyItem(40, ClientID);
        return;
        }},
    {"seli40", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 40, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it40", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 40;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra40", [this, ClientID, pReason]() {
        CreateItem(ClientID, 40, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set41", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 41, Server()->GetItemType(ClientID, 41));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(41 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(41 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon41", [this, ClientID]() {
        BuyItem(41, ClientID);
        return;
        }},
    {"seli41", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 41, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it41", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 41;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra41", [this, ClientID, pReason]() {
        CreateItem(ClientID, 41, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set42", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 42, Server()->GetItemType(ClientID, 42));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(42 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(42 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon42", [this, ClientID]() {
        BuyItem(42, ClientID);
        return;
        }},
    {"seli42", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 42, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it42", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 42;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra42", [this, ClientID, pReason]() {
        CreateItem(ClientID, 42, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set43", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 43, Server()->GetItemType(ClientID, 43));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(43 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(43 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon43", [this, ClientID]() {
        BuyItem(43, ClientID);
        return;
        }},
    {"seli43", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 43, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it43", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 43;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra43", [this, ClientID, pReason]() {
        CreateItem(ClientID, 43, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set44", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 44, Server()->GetItemType(ClientID, 44));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(44 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(44 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon44", [this, ClientID]() {
        BuyItem(44, ClientID);
        return;
        }},
    {"seli44", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 44, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it44", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 44;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra44", [this, ClientID, pReason]() {
        CreateItem(ClientID, 44, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set45", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 45, Server()->GetItemType(ClientID, 45));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(45 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(45 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon45", [this, ClientID]() {
        BuyItem(45, ClientID);
        return;
        }},
    {"seli45", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 45, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it45", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 45;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra45", [this, ClientID, pReason]() {
        CreateItem(ClientID, 45, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set46", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 46, Server()->GetItemType(ClientID, 46));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(46 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(46 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon46", [this, ClientID]() {
        BuyItem(46, ClientID);
        return;
        }},
    {"seli46", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 46, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it46", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 46;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra46", [this, ClientID, pReason]() {
        CreateItem(ClientID, 46, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set47", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 47, Server()->GetItemType(ClientID, 47));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(47 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(47 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon47", [this, ClientID]() {
        BuyItem(47, ClientID);
        return;
        }},
    {"seli47", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 47, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it47", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 47;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra47", [this, ClientID, pReason]() {
        CreateItem(ClientID, 47, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set48", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 48, Server()->GetItemType(ClientID, 48));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(48 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(48 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon48", [this, ClientID]() {
        BuyItem(48, ClientID);
        return;
        }},
    {"seli48", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 48, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it48", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 48;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra48", [this, ClientID, pReason]() {
        CreateItem(ClientID, 48, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set49", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 49, Server()->GetItemType(ClientID, 49));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(49 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(49 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon49", [this, ClientID]() {
        BuyItem(49, ClientID);
        return;
        }},
    {"seli49", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 49, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it49", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 49;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra49", [this, ClientID, pReason]() {
        CreateItem(ClientID, 49, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set50", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 50, Server()->GetItemType(ClientID, 50));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(50 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(50 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon50", [this, ClientID]() {
        BuyItem(50, ClientID);
        return;
        }},
    {"seli50", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 50, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it50", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 50;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra50", [this, ClientID, pReason]() {
        CreateItem(ClientID, 50, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set51", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 51, Server()->GetItemType(ClientID, 51));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(51 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(51 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon51", [this, ClientID]() {
        BuyItem(51, ClientID);
        return;
        }},
    {"seli51", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 51, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it51", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 51;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra51", [this, ClientID, pReason]() {
        CreateItem(ClientID, 51, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set52", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 52, Server()->GetItemType(ClientID, 52));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(52 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(52 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon52", [this, ClientID]() {
        BuyItem(52, ClientID);
        return;
        }},
    {"seli52", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 52, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it52", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 52;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra52", [this, ClientID, pReason]() {
        CreateItem(ClientID, 52, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set53", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 53, Server()->GetItemType(ClientID, 53));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(53 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(53 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon53", [this, ClientID]() {
        BuyItem(53, ClientID);
        return;
        }},
    {"seli53", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 53, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it53", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 53;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra53", [this, ClientID, pReason]() {
        CreateItem(ClientID, 53, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set54", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 54, Server()->GetItemType(ClientID, 54));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(54 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(54 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon54", [this, ClientID]() {
        BuyItem(54, ClientID);
        return;
        }},
    {"seli54", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 54, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it54", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 54;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra54", [this, ClientID, pReason]() {
        CreateItem(ClientID, 54, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set55", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 55, Server()->GetItemType(ClientID, 55));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(55 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(55 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon55", [this, ClientID]() {
        BuyItem(55, ClientID);
        return;
        }},
    {"seli55", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 55, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it55", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 55;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra55", [this, ClientID, pReason]() {
        CreateItem(ClientID, 55, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set56", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 56, Server()->GetItemType(ClientID, 56));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(56 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(56 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon56", [this, ClientID]() {
        BuyItem(56, ClientID);
        return;
        }},
    {"seli56", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 56, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it56", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 56;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra56", [this, ClientID, pReason]() {
        CreateItem(ClientID, 56, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set57", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 57, Server()->GetItemType(ClientID, 57));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(57 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(57 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon57", [this, ClientID]() {
        BuyItem(57, ClientID);
        return;
        }},
    {"seli57", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 57, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it57", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 57;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra57", [this, ClientID, pReason]() {
        CreateItem(ClientID, 57, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set58", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 58, Server()->GetItemType(ClientID, 58));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(58 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(58 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon58", [this, ClientID]() {
        BuyItem(58, ClientID);
        return;
        }},
    {"seli58", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 58, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it58", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 58;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra58", [this, ClientID, pReason]() {
        CreateItem(ClientID, 58, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set59", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 59, Server()->GetItemType(ClientID, 59));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(59 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(59 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon59", [this, ClientID]() {
        BuyItem(59, ClientID);
        return;
        }},
    {"seli59", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 59, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it59", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 59;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra59", [this, ClientID, pReason]() {
        CreateItem(ClientID, 59, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set60", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 60, Server()->GetItemType(ClientID, 60));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(60 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(60 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon60", [this, ClientID]() {
        BuyItem(60, ClientID);
        return;
        }},
    {"seli60", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 60, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it60", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 60;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra60", [this, ClientID, pReason]() {
        CreateItem(ClientID, 60, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set61", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 61, Server()->GetItemType(ClientID, 61));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(61 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(61 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon61", [this, ClientID]() {
        BuyItem(61, ClientID);
        return;
        }},
    {"seli61", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 61, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it61", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 61;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra61", [this, ClientID, pReason]() {
        CreateItem(ClientID, 61, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set62", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 62, Server()->GetItemType(ClientID, 62));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(62 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(62 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon62", [this, ClientID]() {
        BuyItem(62, ClientID);
        return;
        }},
    {"seli62", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 62, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it62", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 62;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra62", [this, ClientID, pReason]() {
        CreateItem(ClientID, 62, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set63", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 63, Server()->GetItemType(ClientID, 63));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(63 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(63 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon63", [this, ClientID]() {
        BuyItem(63, ClientID);
        return;
        }},
    {"seli63", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 63, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it63", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 63;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra63", [this, ClientID, pReason]() {
        CreateItem(ClientID, 63, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set64", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 64, Server()->GetItemType(ClientID, 64));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(64 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(64 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon64", [this, ClientID]() {
        BuyItem(64, ClientID);
        return;
        }},
    {"seli64", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 64, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it64", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 64;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra64", [this, ClientID, pReason]() {
        CreateItem(ClientID, 64, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set65", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 65, Server()->GetItemType(ClientID, 65));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(65 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(65 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon65", [this, ClientID]() {
        BuyItem(65, ClientID);
        return;
        }},
    {"seli65", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 65, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it65", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 65;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra65", [this, ClientID, pReason]() {
        CreateItem(ClientID, 65, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set66", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 66, Server()->GetItemType(ClientID, 66));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(66 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(66 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon66", [this, ClientID]() {
        BuyItem(66, ClientID);
        return;
        }},
    {"seli66", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 66, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it66", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 66;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra66", [this, ClientID, pReason]() {
        CreateItem(ClientID, 66, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set67", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 67, Server()->GetItemType(ClientID, 67));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(67 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(67 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon67", [this, ClientID]() {
        BuyItem(67, ClientID);
        return;
        }},
    {"seli67", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 67, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it67", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 67;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra67", [this, ClientID, pReason]() {
        CreateItem(ClientID, 67, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set68", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 68, Server()->GetItemType(ClientID, 68));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(68 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(68 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon68", [this, ClientID]() {
        BuyItem(68, ClientID);
        return;
        }},
    {"seli68", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 68, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it68", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 68;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra68", [this, ClientID, pReason]() {
        CreateItem(ClientID, 68, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set69", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 69, Server()->GetItemType(ClientID, 69));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(69 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(69 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon69", [this, ClientID]() {
        BuyItem(69, ClientID);
        return;
        }},
    {"seli69", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 69, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it69", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 69;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra69", [this, ClientID, pReason]() {
        CreateItem(ClientID, 69, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set70", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 70, Server()->GetItemType(ClientID, 70));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(70 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(70 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon70", [this, ClientID]() {
        BuyItem(70, ClientID);
        return;
        }},
    {"seli70", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 70, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it70", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 70;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra70", [this, ClientID, pReason]() {
        CreateItem(ClientID, 70, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set71", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 71, Server()->GetItemType(ClientID, 71));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(71 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(71 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon71", [this, ClientID]() {
        BuyItem(71, ClientID);
        return;
        }},
    {"seli71", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 71, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it71", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 71;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra71", [this, ClientID, pReason]() {
        CreateItem(ClientID, 71, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set72", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 72, Server()->GetItemType(ClientID, 72));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(72 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(72 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon72", [this, ClientID]() {
        BuyItem(72, ClientID);
        return;
        }},
    {"seli72", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 72, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it72", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 72;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra72", [this, ClientID, pReason]() {
        CreateItem(ClientID, 72, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set73", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 73, Server()->GetItemType(ClientID, 73));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(73 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(73 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon73", [this, ClientID]() {
        BuyItem(73, ClientID);
        return;
        }},
    {"seli73", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 73, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it73", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 73;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra73", [this, ClientID, pReason]() {
        CreateItem(ClientID, 73, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set74", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 74, Server()->GetItemType(ClientID, 74));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(74 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(74 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon74", [this, ClientID]() {
        BuyItem(74, ClientID);
        return;
        }},
    {"seli74", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 74, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it74", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 74;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra74", [this, ClientID, pReason]() {
        CreateItem(ClientID, 74, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set75", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 75, Server()->GetItemType(ClientID, 75));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(75 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(75 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon75", [this, ClientID]() {
        BuyItem(75, ClientID);
        return;
        }},
    {"seli75", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 75, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it75", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 75;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra75", [this, ClientID, pReason]() {
        CreateItem(ClientID, 75, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set76", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 76, Server()->GetItemType(ClientID, 76));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(76 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(76 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon76", [this, ClientID]() {
        BuyItem(76, ClientID);
        return;
        }},
    {"seli76", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 76, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it76", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 76;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra76", [this, ClientID, pReason]() {
        CreateItem(ClientID, 76, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set77", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 77, Server()->GetItemType(ClientID, 77));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(77 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(77 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon77", [this, ClientID]() {
        BuyItem(77, ClientID);
        return;
        }},
    {"seli77", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 77, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it77", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 77;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra77", [this, ClientID, pReason]() {
        CreateItem(ClientID, 77, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set78", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 78, Server()->GetItemType(ClientID, 78));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(78 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(78 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon78", [this, ClientID]() {
        BuyItem(78, ClientID);
        return;
        }},
    {"seli78", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 78, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it78", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 78;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra78", [this, ClientID, pReason]() {
        CreateItem(ClientID, 78, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set79", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 79, Server()->GetItemType(ClientID, 79));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(79 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(79 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon79", [this, ClientID]() {
        BuyItem(79, ClientID);
        return;
        }},
    {"seli79", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 79, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it79", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 79;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra79", [this, ClientID, pReason]() {
        CreateItem(ClientID, 79, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set80", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 80, Server()->GetItemType(ClientID, 80));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(80 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(80 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon80", [this, ClientID]() {
        BuyItem(80, ClientID);
        return;
        }},
    {"seli80", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 80, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it80", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 80;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra80", [this, ClientID, pReason]() {
        CreateItem(ClientID, 80, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set81", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 81, Server()->GetItemType(ClientID, 81));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(81 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(81 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon81", [this, ClientID]() {
        BuyItem(81, ClientID);
        return;
        }},
    {"seli81", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 81, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it81", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 81;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra81", [this, ClientID, pReason]() {
        CreateItem(ClientID, 81, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set82", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 82, Server()->GetItemType(ClientID, 82));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(82 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(82 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon82", [this, ClientID]() {
        BuyItem(82, ClientID);
        return;
        }},
    {"seli82", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 82, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it82", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 82;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra82", [this, ClientID, pReason]() {
        CreateItem(ClientID, 82, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set83", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 83, Server()->GetItemType(ClientID, 83));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(83 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(83 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon83", [this, ClientID]() {
        BuyItem(83, ClientID);
        return;
        }},
    {"seli83", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 83, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it83", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 83;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra83", [this, ClientID, pReason]() {
        CreateItem(ClientID, 83, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set84", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 84, Server()->GetItemType(ClientID, 84));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(84 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(84 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon84", [this, ClientID]() {
        BuyItem(84, ClientID);
        return;
        }},
    {"seli84", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 84, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it84", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 84;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra84", [this, ClientID, pReason]() {
        CreateItem(ClientID, 84, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set85", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 85, Server()->GetItemType(ClientID, 85));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(85 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(85 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon85", [this, ClientID]() {
        BuyItem(85, ClientID);
        return;
        }},
    {"seli85", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 85, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it85", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 85;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra85", [this, ClientID, pReason]() {
        CreateItem(ClientID, 85, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set86", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 86, Server()->GetItemType(ClientID, 86));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(86 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(86 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon86", [this, ClientID]() {
        BuyItem(86, ClientID);
        return;
        }},
    {"seli86", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 86, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it86", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 86;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra86", [this, ClientID, pReason]() {
        CreateItem(ClientID, 86, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set87", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 87, Server()->GetItemType(ClientID, 87));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(87 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(87 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon87", [this, ClientID]() {
        BuyItem(87, ClientID);
        return;
        }},
    {"seli87", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 87, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it87", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 87;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra87", [this, ClientID, pReason]() {
        CreateItem(ClientID, 87, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set88", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 88, Server()->GetItemType(ClientID, 88));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(88 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(88 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon88", [this, ClientID]() {
        BuyItem(88, ClientID);
        return;
        }},
    {"seli88", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 88, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it88", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 88;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra88", [this, ClientID, pReason]() {
        CreateItem(ClientID, 88, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set89", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 89, Server()->GetItemType(ClientID, 89));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(89 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(89 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon89", [this, ClientID]() {
        BuyItem(89, ClientID);
        return;
        }},
    {"seli89", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 89, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it89", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 89;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra89", [this, ClientID, pReason]() {
        CreateItem(ClientID, 89, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set90", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 90, Server()->GetItemType(ClientID, 90));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(90 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(90 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon90", [this, ClientID]() {
        BuyItem(90, ClientID);
        return;
        }},
    {"seli90", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 90, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it90", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 90;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra90", [this, ClientID, pReason]() {
        CreateItem(ClientID, 90, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set91", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 91, Server()->GetItemType(ClientID, 91));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(91 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(91 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon91", [this, ClientID]() {
        BuyItem(91, ClientID);
        return;
        }},
    {"seli91", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 91, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it91", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 91;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra91", [this, ClientID, pReason]() {
        CreateItem(ClientID, 91, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set92", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 92, Server()->GetItemType(ClientID, 92));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(92 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(92 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon92", [this, ClientID]() {
        BuyItem(92, ClientID);
        return;
        }},
    {"seli92", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 92, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it92", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 92;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra92", [this, ClientID, pReason]() {
        CreateItem(ClientID, 92, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set93", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 93, Server()->GetItemType(ClientID, 93));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(93 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(93 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon93", [this, ClientID]() {
        BuyItem(93, ClientID);
        return;
        }},
    {"seli93", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 93, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it93", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 93;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra93", [this, ClientID, pReason]() {
        CreateItem(ClientID, 93, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set94", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 94, Server()->GetItemType(ClientID, 94));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(94 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(94 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon94", [this, ClientID]() {
        BuyItem(94, ClientID);
        return;
        }},
    {"seli94", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 94, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it94", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 94;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra94", [this, ClientID, pReason]() {
        CreateItem(ClientID, 94, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set95", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 95, Server()->GetItemType(ClientID, 95));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(95 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(95 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon95", [this, ClientID]() {
        BuyItem(95, ClientID);
        return;
        }},
    {"seli95", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 95, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it95", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 95;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra95", [this, ClientID, pReason]() {
        CreateItem(ClientID, 95, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set96", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 96, Server()->GetItemType(ClientID, 96));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(96 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(96 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon96", [this, ClientID]() {
        BuyItem(96, ClientID);
        return;
        }},
    {"seli96", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 96, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it96", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 96;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra96", [this, ClientID, pReason]() {
        CreateItem(ClientID, 96, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set97", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 97, Server()->GetItemType(ClientID, 97));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(97 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(97 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon97", [this, ClientID]() {
        BuyItem(97, ClientID);
        return;
        }},
    {"seli97", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 97, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it97", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 97;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra97", [this, ClientID, pReason]() {
        CreateItem(ClientID, 97, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set98", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 98, Server()->GetItemType(ClientID, 98));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(98 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(98 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon98", [this, ClientID]() {
        BuyItem(98, ClientID);
        return;
        }},
    {"seli98", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 98, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it98", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 98;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra98", [this, ClientID, pReason]() {
        CreateItem(ClientID, 98, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set99", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 99, Server()->GetItemType(ClientID, 99));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(99 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(99 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon99", [this, ClientID]() {
        BuyItem(99, ClientID);
        return;
        }},
    {"seli99", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 99, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it99", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 99;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra99", [this, ClientID, pReason]() {
        CreateItem(ClientID, 99, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set100", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 100, Server()->GetItemType(ClientID, 100));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(100 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(100 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon100", [this, ClientID]() {
        BuyItem(100, ClientID);
        return;
        }},
    {"seli100", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 100, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it100", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 100;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra100", [this, ClientID, pReason]() {
        CreateItem(ClientID, 100, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set101", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 101, Server()->GetItemType(ClientID, 101));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(101 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(101 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon101", [this, ClientID]() {
        BuyItem(101, ClientID);
        return;
        }},
    {"seli101", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 101, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it101", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 101;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra101", [this, ClientID, pReason]() {
        CreateItem(ClientID, 101, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set102", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 102, Server()->GetItemType(ClientID, 102));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(102 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(102 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon102", [this, ClientID]() {
        BuyItem(102, ClientID);
        return;
        }},
    {"seli102", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 102, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it102", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 102;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra102", [this, ClientID, pReason]() {
        CreateItem(ClientID, 102, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set103", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 103, Server()->GetItemType(ClientID, 103));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(103 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(103 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon103", [this, ClientID]() {
        BuyItem(103, ClientID);
        return;
        }},
    {"seli103", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 103, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it103", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 103;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra103", [this, ClientID, pReason]() {
        CreateItem(ClientID, 103, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set104", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 104, Server()->GetItemType(ClientID, 104));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(104 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(104 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon104", [this, ClientID]() {
        BuyItem(104, ClientID);
        return;
        }},
    {"seli104", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 104, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it104", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 104;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra104", [this, ClientID, pReason]() {
        CreateItem(ClientID, 104, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set105", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 105, Server()->GetItemType(ClientID, 105));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(105 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(105 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon105", [this, ClientID]() {
        BuyItem(105, ClientID);
        return;
        }},
    {"seli105", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 105, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it105", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 105;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra105", [this, ClientID, pReason]() {
        CreateItem(ClientID, 105, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set106", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 106, Server()->GetItemType(ClientID, 106));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(106 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(106 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon106", [this, ClientID]() {
        BuyItem(106, ClientID);
        return;
        }},
    {"seli106", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 106, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it106", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 106;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra106", [this, ClientID, pReason]() {
        CreateItem(ClientID, 106, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set107", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 107, Server()->GetItemType(ClientID, 107));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(107 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(107 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon107", [this, ClientID]() {
        BuyItem(107, ClientID);
        return;
        }},
    {"seli107", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 107, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it107", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 107;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra107", [this, ClientID, pReason]() {
        CreateItem(ClientID, 107, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set108", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 108, Server()->GetItemType(ClientID, 108));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(108 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(108 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon108", [this, ClientID]() {
        BuyItem(108, ClientID);
        return;
        }},
    {"seli108", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 108, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it108", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 108;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra108", [this, ClientID, pReason]() {
        CreateItem(ClientID, 108, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set109", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 109, Server()->GetItemType(ClientID, 109));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(109 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(109 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon109", [this, ClientID]() {
        BuyItem(109, ClientID);
        return;
        }},
    {"seli109", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 109, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it109", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 109;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra109", [this, ClientID, pReason]() {
        CreateItem(ClientID, 109, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set110", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 110, Server()->GetItemType(ClientID, 110));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(110 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(110 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon110", [this, ClientID]() {
        BuyItem(110, ClientID);
        return;
        }},
    {"seli110", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 110, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it110", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 110;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra110", [this, ClientID, pReason]() {
        CreateItem(ClientID, 110, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set111", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 111, Server()->GetItemType(ClientID, 111));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(111 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(111 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon111", [this, ClientID]() {
        BuyItem(111, ClientID);
        return;
        }},
    {"seli111", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 111, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it111", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 111;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra111", [this, ClientID, pReason]() {
        CreateItem(ClientID, 111, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set112", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 112, Server()->GetItemType(ClientID, 112));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(112 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(112 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon112", [this, ClientID]() {
        BuyItem(112, ClientID);
        return;
        }},
    {"seli112", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 112, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it112", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 112;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra112", [this, ClientID, pReason]() {
        CreateItem(ClientID, 112, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set113", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 113, Server()->GetItemType(ClientID, 113));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(113 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(113 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon113", [this, ClientID]() {
        BuyItem(113, ClientID);
        return;
        }},
    {"seli113", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 113, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it113", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 113;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra113", [this, ClientID, pReason]() {
        CreateItem(ClientID, 113, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set114", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 114, Server()->GetItemType(ClientID, 114));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(114 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(114 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon114", [this, ClientID]() {
        BuyItem(114, ClientID);
        return;
        }},
    {"seli114", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 114, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it114", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 114;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra114", [this, ClientID, pReason]() {
        CreateItem(ClientID, 114, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set115", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 115, Server()->GetItemType(ClientID, 115));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(115 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(115 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon115", [this, ClientID]() {
        BuyItem(115, ClientID);
        return;
        }},
    {"seli115", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 115, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it115", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 115;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra115", [this, ClientID, pReason]() {
        CreateItem(ClientID, 115, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set116", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 116, Server()->GetItemType(ClientID, 116));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(116 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(116 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon116", [this, ClientID]() {
        BuyItem(116, ClientID);
        return;
        }},
    {"seli116", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 116, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it116", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 116;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra116", [this, ClientID, pReason]() {
        CreateItem(ClientID, 116, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set117", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 117, Server()->GetItemType(ClientID, 117));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(117 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(117 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon117", [this, ClientID]() {
        BuyItem(117, ClientID);
        return;
        }},
    {"seli117", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 117, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it117", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 117;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra117", [this, ClientID, pReason]() {
        CreateItem(ClientID, 117, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set118", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 118, Server()->GetItemType(ClientID, 118));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(118 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(118 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon118", [this, ClientID]() {
        BuyItem(118, ClientID);
        return;
        }},
    {"seli118", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 118, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it118", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 118;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra118", [this, ClientID, pReason]() {
        CreateItem(ClientID, 118, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set119", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 119, Server()->GetItemType(ClientID, 119));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(119 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(119 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon119", [this, ClientID]() {
        BuyItem(119, ClientID);
        return;
        }},
    {"seli119", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 119, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it119", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 119;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra119", [this, ClientID, pReason]() {
        CreateItem(ClientID, 119, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set120", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 120, Server()->GetItemType(ClientID, 120));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(120 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(120 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon120", [this, ClientID]() {
        BuyItem(120, ClientID);
        return;
        }},
    {"seli120", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 120, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it120", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 120;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra120", [this, ClientID, pReason]() {
        CreateItem(ClientID, 120, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set121", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 121, Server()->GetItemType(ClientID, 121));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(121 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(121 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon121", [this, ClientID]() {
        BuyItem(121, ClientID);
        return;
        }},
    {"seli121", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 121, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it121", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 121;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra121", [this, ClientID, pReason]() {
        CreateItem(ClientID, 121, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set122", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 122, Server()->GetItemType(ClientID, 122));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(122 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(122 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon122", [this, ClientID]() {
        BuyItem(122, ClientID);
        return;
        }},
    {"seli122", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 122, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it122", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 122;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra122", [this, ClientID, pReason]() {
        CreateItem(ClientID, 122, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set123", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 123, Server()->GetItemType(ClientID, 123));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(123 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(123 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon123", [this, ClientID]() {
        BuyItem(123, ClientID);
        return;
        }},
    {"seli123", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 123, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it123", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 123;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra123", [this, ClientID, pReason]() {
        CreateItem(ClientID, 123, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set124", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 124, Server()->GetItemType(ClientID, 124));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(124 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(124 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon124", [this, ClientID]() {
        BuyItem(124, ClientID);
        return;
        }},
    {"seli124", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 124, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it124", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 124;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra124", [this, ClientID, pReason]() {
        CreateItem(ClientID, 124, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set125", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 125, Server()->GetItemType(ClientID, 125));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(125 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(125 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon125", [this, ClientID]() {
        BuyItem(125, ClientID);
        return;
        }},
    {"seli125", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 125, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it125", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 125;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra125", [this, ClientID, pReason]() {
        CreateItem(ClientID, 125, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set126", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 126, Server()->GetItemType(ClientID, 126));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(126 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(126 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon126", [this, ClientID]() {
        BuyItem(126, ClientID);
        return;
        }},
    {"seli126", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 126, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it126", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 126;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra126", [this, ClientID, pReason]() {
        CreateItem(ClientID, 126, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set127", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 127, Server()->GetItemType(ClientID, 127));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(127 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(127 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon127", [this, ClientID]() {
        BuyItem(127, ClientID);
        return;
        }},
    {"seli127", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 127, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it127", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 127;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra127", [this, ClientID, pReason]() {
        CreateItem(ClientID, 127, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set128", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 128, Server()->GetItemType(ClientID, 128));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(128 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(128 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon128", [this, ClientID]() {
        BuyItem(128, ClientID);
        return;
        }},
    {"seli128", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 128, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it128", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 128;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra128", [this, ClientID, pReason]() {
        CreateItem(ClientID, 128, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set129", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 129, Server()->GetItemType(ClientID, 129));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(129 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(129 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon129", [this, ClientID]() {
        BuyItem(129, ClientID);
        return;
        }},
    {"seli129", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 129, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it129", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 129;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra129", [this, ClientID, pReason]() {
        CreateItem(ClientID, 129, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set130", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 130, Server()->GetItemType(ClientID, 130));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(130 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(130 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon130", [this, ClientID]() {
        BuyItem(130, ClientID);
        return;
        }},
    {"seli130", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 130, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it130", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 130;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra130", [this, ClientID, pReason]() {
        CreateItem(ClientID, 130, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"set131", [this, ClientID]() {
        Server()->SetItemSettings(ClientID, 131, Server()->GetItemType(ClientID, 131));	
        ResetVotes(ClientID, m_apPlayers[ClientID]->m_UpdateMenu);	

        m_pController->OnPlayerInfoChange(m_apPlayers[ClientID]);

        if(131 == PIZDAMET)
        {	
            if(Server()->GetItemSettings(ClientID, PIZDAMET))
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, 7000);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_GRENADE, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*8));
        }

        if(131 == LAMPHAMMER)
        {	
            if(Server()->GetItemSettings(ClientID, LAMPHAMMER))
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, 1200);
            else
                Server()->SetFireDelay(ClientID, INFWEAPON_HAMMER, int(1000+m_apPlayers[ClientID]->AccUpgrade.Speed*12));
        }
        return;
        }},
    {"bon131", [this, ClientID]() {
        BuyItem(131, ClientID);
        return;
        }},
    {"seli131", [this, ClientID, pReason]() {
        Server()->RemItem(ClientID, 131, chartoint(pReason, MAX_COUNT), USEDSELL);
        return;	
        }},
    {"it131", [this, ClientID]() {
        m_apPlayers[ClientID]->m_SelectItem = 131;
        ResetVotes(ClientID, SELITEM);
        return;	
        }},
    {"cra131", [this, ClientID, pReason]() {
        CreateItem(ClientID, 131, chartoint(pReason, MAX_COUNT));
        return;	
        }},
    {"cs0", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 0), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs1", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 1), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs2", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 2), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs3", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 3), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs4", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 4), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs5", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 5), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs6", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 6), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs7", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 7), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs8", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 8), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs9", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 9), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs10", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 10), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs11", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 11), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs12", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 12), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs13", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 13), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs14", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 14), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs15", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 15), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs16", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 16), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs17", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 17), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs18", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 18), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs19", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 19), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs20", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 20), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs21", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 21), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs22", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 22), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs23", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 23), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs24", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 24), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs25", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 25), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs26", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 26), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs27", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 27), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs28", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 28), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs29", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 29), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs30", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 30), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs31", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 31), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs32", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 32), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs33", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 33), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs34", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 34), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs35", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 35), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs36", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 36), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs37", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 37), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs38", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 38), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs39", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 39), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs40", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 40), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs41", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 41), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs42", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 42), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs43", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 43), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs44", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 44), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs45", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 45), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs46", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 46), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs47", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 47), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs48", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 48), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs49", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 49), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs50", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 50), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs51", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 51), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs52", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 52), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs53", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 53), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs54", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 54), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs55", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 55), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs56", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 56), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs57", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 57), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs58", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 58), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs59", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 59), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs60", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 60), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs61", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 61), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs62", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 62), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }},
    {"cs63", [this, ClientID]() {
        str_copy(m_apPlayers[ClientID]->m_SelectPlayer, Server()->GetSelectName(ClientID, 63), sizeof(m_apPlayers[ClientID]->m_SelectPlayer));
        ResetVotes(ClientID, CSETTING);
        return;
        }}
};
