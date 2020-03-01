/***************************************************************************
 *                                                                         *
 * Uzak Diyarlar a��k kaynak T�rk�e Mud projesidir.                        *
 * Oyun geli�tirmesi Jai ve Maru taraf�ndan y�netilmektedir.               *
 * Unutulmamas� gerekenler: Nir, Kame, Nyah, Sint                          *
 *                                                                         *
 * Github  : https://github.com/yelbuke/UzakDiyarlar                       *
 * Web     : http://www.uzakdiyarlar.net                                   *
 * Discord : https://discord.gg/kXyZzv                                     *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 *     ANATOLIA 2.1 is copyright 1996-1997 Serdar BULUT		           *
 *     ANATOLIA has been brought to you by ANATOLIA consortium		   *
 *	 Serdar BULUT {Chronos}		bulut@rorqual.cc.metu.edu.tr       *
 *	 Ibrahim Canpunar  {Mandrake}	canpunar@rorqual.cc.metu.edu.tr    *
 *	 Murat BICER  {KIO}		mbicer@rorqual.cc.metu.edu.tr	   *
 *	 D.Baris ACAR {Powerman}	dbacar@rorqual.cc.metu.edu.tr	   *
 *     By using this code, you have agreed to follow the terms of the      *
 *     ANATOLIA license, in the file Anatolia/anatolia.licence             *
 ***************************************************************************/

/****************************************************************************
*  Automated Quest code written by Vassago of MOONGATE, moongate.ams.com    *
*  4000. Copyright (c) 1996 Ryan Addams, All Rights Reserved. Use of this   *
*  code is allowed provided you add a credit line to the effect of:         *
*  "Quest Code (c) 1996 Ryan Addams" to your logon screen with the rest     *
*  of the standard diku/rom credits. If you use this or a modified version  *
*  of this code, let me know via email: moongate@moongate.ams.com. Further  *
*  updates will be posted to the rom mailing list. If you'd like to get     *
*  the latest version of quest.c, please send a request to the above add-   *
*  ress. Quest Code v2.01. Please do not remove this notice from this file. *
****************************************************************************/

/****************************************************************************
 *  Revised and adopted to Anatolia by chronos.                             *
 ****************************************************************************/

/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*       ROM 2.4 is copyright 1993-1995 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"

DECLARE_SPELL_FUN(	spell_identify	);

void do_tell_quest( CHAR_DATA *ch, CHAR_DATA *victim, char *argument);
CHAR_DATA *get_quest_world( CHAR_DATA *ch, MOB_INDEX_DATA *victim );
extern	MOB_INDEX_DATA	*mob_index_hash	[MAX_KEY_HASH];

/* Object vnums for object quest 'tokens': object quest. */

#define QUEST_OBJQUEST1 84
#define QUEST_OBJQUEST2 85
#define QUEST_OBJQUEST3 86
#define QUEST_OBJQUEST4	97

/* Local functions */

void generate_quest	args(( CHAR_DATA *ch, CHAR_DATA *questman ));
void quest_update	args(( void ));
bool chance		args(( int num ));

/* CHANCE function. I use this everywhere in my code, very handy :> */

bool chance(int num)
{
    if (number_range(1,100) <= num) return TRUE;
    else return FALSE;
}

/* The main quest function */

void do_quest(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *questman;
    OBJ_DATA *obj=NULL, *obj_next;
    OBJ_INDEX_DATA *questinfoobj;
    MOB_INDEX_DATA *questinfo;
    ROOM_INDEX_DATA *pRoomIndex;
    char buf [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    int sn,trouble_vnum=0;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (IS_NPC(ch) )  return;

  if ( IS_SET(ch->act,PLR_NO_DESCRIPTION) )
  {
    printf_to_char(ch, "En az 350 karakterlik tan�m�n olmadan g�rev komutlar�n� kullanamazs�n.");
    return;
  }

  if (!strcmp(arg1, "bilgi"))
  {
    if (IS_SET(ch->act, PLR_QUESTOR))
    {
      if (ch->pcdata->questmob == -1 )
      {
        send_to_char("G�revin neredeyse tamamland�!\n\rZaman�n bitmeden �nce g�revciye git!\n\r", ch);
      }
      else if (ch->pcdata->questobj > 0)
      {
        questinfoobj = get_obj_index(ch->pcdata->questobj);
        if (questinfoobj != NULL)
        {
          pRoomIndex = get_room_index(ch->pcdata->questroom);
          printf_to_char(ch,"G�revin, {c%s{w b�lgesinde {c%s{w isimli odadaki {c%s{w e�yas�n� bulmak!\n\r",
            pRoomIndex->area->name,pRoomIndex->name,questinfoobj->name);
        }
        else
          send_to_char("Hen�z bir g�revin yok.\n\r",ch);
        return;
      }
      else if (ch->pcdata->questmob > 0)
      {
        questinfo = get_mob_index(ch->pcdata->questmob);
        if (questinfo != NULL)
        {
          pRoomIndex = get_room_index(ch->pcdata->questroom);
          printf_to_char(ch,"G�revin, {c%s{w b�lgesinde {c%s{w isimli odadaki {c%s{w adl� korkun� yarat��� �ld�rmek!\n\r",
            pRoomIndex->area->name,pRoomIndex->name,questinfo->short_descr);
        }
        else
          send_to_char("Hen�z bir g�revin yok.\n\r",ch);
        return;
      }
    }
    else
    send_to_char("Hen�z bir g�revin yok.\n\r",ch);
    return;
  }
    if (!strcmp(arg1, "puan"))
    {
      sprintf(buf, "%d g�rev puan�n var.\n\r",ch->pcdata->questpoints);
	send_to_char(buf, ch);
	return;
    }
    else if (!strcmp(arg1, "zaman"))
    {
	if (!IS_SET(ch->act, PLR_QUESTOR))
	{
    send_to_char("Hen�z bir g�revin yok.\n\r",ch);
	    if (ch->pcdata->nextquest > 1)
	    {
        sprintf(buf, "Yeni bir g�rev isteyebilmen i�in %d dakika kald�.\n\r",ch->pcdata->nextquest);
		send_to_char(buf, ch);
	    }
	    else if (ch->pcdata->nextquest == 1)
	    {
        sprintf(buf, "Yeni bir g�rev isteyebilmen i�in bir dakikadan az zaman kald�.\n\r");
		send_to_char(buf, ch);
	    }
	}
        else if (ch->pcdata->countdown > 0)
        {
          sprintf(buf, "G�revi bitirmek i�in kalan zaman: %d dakika.\n\r",ch->pcdata->countdown);
	    send_to_char(buf, ch);
	}
	return;
    }

/* Checks for a character in the room with spec_questmaster set. This special
   procedure must be defined in special.c. You could instead use an
   ACT_QUESTMASTER flag instead of a special procedure. */

    for ( questman = ch->in_room->people; questman != NULL; questman = questman->next_in_room )
    {
	if (!IS_NPC(questman)) continue;
        if (questman->spec_fun == spec_lookup( (char*)"spec_questmaster" )) break;
    }

    if (questman == NULL || questman->spec_fun != spec_lookup( (char*)"spec_questmaster" ))
    {
      send_to_char("Odada g�revci g�remiyorum.\n\r", ch);
        return;
    }

    if ( questman->fighting != NULL)
    {
      send_to_char("D�v�� bitene kadar bekle.\n\r",ch);
        return;
    }

    ch->pcdata->questgiver = questman->pIndexData->vnum;

/* And, of course, you will need to change the following lines for YOUR
   quest item information. Quest items on Moongate are unbalanced, very
   very nice items, and no one has one yet, because it takes awhile to
   build up quest points :> Make the item worth their while. */

    if (!strcmp(arg1, "liste"))
    {
      act("$n $Z g�rev ekipmanlar�n�n listesini istiyor.", ch, NULL, questman, TO_ROOM);
act ("$Z g�rev ekipmanlar�n�n listesini istiyorsun.",ch, NULL, questman, TO_CHAR);

printf_to_char(ch, "{Cs�rt�antas�{x.........({y�anta{x   )......{R5000 gp{x\n\r");
printf_to_char(ch, "{Ckemer{x...............({ykemer{x   )......{R1000 gp{x\n\r");
printf_to_char(ch, "{Cparlak silah{x........({yparlak{x  )......{R1000 gp{x\n\r");
printf_to_char(ch, "{Cmat silah{x...........({ymat{x     )......{R1000 gp{x\n\r");
printf_to_char(ch, "{C100 pratik{x..........({ypratik{x  )......{R1000 gp{x\n\r");
printf_to_char(ch, "{Ci�lemeli y�z�k{x......({yi�lemeli{x).......{R750 gp{x\n\r");
printf_to_char(ch, "{Cdesenli y�z�k{x.......({ydesenli{x ).......{R750 gp{x\n\r");
printf_to_char(ch, "{Coymal� y�z�k{x........({yoymal�{x  ).......{R750 gp{x\n\r");
printf_to_char(ch, "{Ckakmal� y�z�k{x.......({ykakmal�{x ).......{R750 gp{x\n\r");
printf_to_char(ch, "{Ctesti{x...............({ytesti{x   ).......{R500 gp{x\n\r");
printf_to_char(ch, "{C350.000 alt�n{x.......({yalt�n{x   ).......{R500 gp{x\n\r");
printf_to_char(ch, "{C1 b�nye puan�{x.......({yb�nye{x   ).......{R250 gp{x\n\r");
if ( ch->iclass == CLASS_SAMURAI )
{
printf_to_char(ch, "{Ckatana{x..............({ykatana{x  ).......{R100 gp{x\n\r");
printf_to_char(ch, "{Cikinci katana{x.......({ykeskin{x  ).......{R100 gp{x\n\r");
printf_to_char(ch, "{C�l�m azaltma{x........({y�l�m{x    )........{R50 gp{x\n\r");
}
if ( ch->iclass == CLASS_VAMPIRE )
{
printf_to_char(ch, "{Cvampir yetene�i{x.....({yvampir{x  )........{R50 gp{x\n\r");
}

printf_to_char(ch, "Bir e�ya sat�n almak i�in {Rg�rev sat�nal <e�ya_ad�>{x yaz.\n\r");
	return;
    }
    else if (!strcmp(arg1, "�zellik"))
  	{
  		if (arg2[0] == '\0')
  		{
  			printf_to_char(ch,"Hangi g�rev ekipman�n�n �zelliklerini ��renmek istiyorsun?\n\rKullan�m: {Rg�rev �zellik <ekipman>{x\n\r");
  			return;
  		}
  		else if (is_name(arg2, (char*)"�anta"))
  		{
  			obj = create_object(get_obj_index(QUEST_ITEM4),ch->level);
  			spell_identify( 0, 0, ch, obj ,0);
  			extract_obj(obj);
  			return;
  		}
  		else if (is_name(arg2, (char*)"kemer"))
  		{
  			obj = create_object(get_obj_index(QUEST_ITEM1),ch->level);
  			spell_identify( 0, 0, ch, obj ,0);
  			extract_obj(obj);
  			return;
  		}
  		else if (is_name(arg2, (char*)"parlak"))
  		{
  			obj = create_object(get_obj_index(QUEST_ITEM_SILAH1),ch->level);
  			spell_identify( 0, 0, ch, obj ,0);
  			extract_obj(obj);
  			return;
  		}
  		else if (is_name(arg2, (char*)"mat"))
  		{
  			obj = create_object(get_obj_index(QUEST_ITEM_SILAH2),ch->level);
  			spell_identify( 0, 0, ch, obj ,0);
  			extract_obj(obj);
  			return;
  		}
  		else if (is_name(arg2, (char*)"i�lemeli"))
  		{
  			obj = create_object(get_obj_index(QUEST_ITEM_YUZUK1),ch->level);
  			spell_identify( 0, 0, ch, obj ,0);
  			extract_obj(obj);
  			return;
  		}
  		else if (is_name(arg2, (char*)"desenli"))
  		{
  			obj = create_object(get_obj_index(QUEST_ITEM_YUZUK2),ch->level);
  			spell_identify( 0, 0, ch, obj ,0);
  			extract_obj(obj);
  			return;
  		}
  		else if (is_name(arg2, (char*)"oymal�"))
  		{
  			obj = create_object(get_obj_index(QUEST_ITEM_YUZUK3),ch->level);
  			spell_identify( 0, 0, ch, obj ,0);
  			extract_obj(obj);
  			return;
  		}
      else if (is_name(arg2, (char*)"kakmal�"))
  		{
  			obj = create_object(get_obj_index(QUEST_ITEM_YUZUK4),ch->level);
  			spell_identify( 0, 0, ch, obj ,0);
  			extract_obj(obj);
  			return;
  		}
      else if (is_name(arg2, (char*)"testi"))
  		{
  			obj = create_object(get_obj_index(QUEST_ITEM5),ch->level);
  			spell_identify( 0, 0, ch, obj ,0);
  			extract_obj(obj);
  			return;
  		}
  		else
  		{
  			printf_to_char(ch,"Hangi g�rev ekipman�n�n �zelliklerini ��renmek istiyorsun? {Rg�rev �zellik <ekipman>{x\n\r");
  			return;
  		}
  	}

    else if (!strcmp(arg1, "sat�nal"))
    {
	if (arg2[0] == '\0')
	{
    send_to_char("Bir �d�l� sat�n almak i�in 'g�rev sat�nal <g�rev_e�yas�>' yaz�n.\n\r",ch);
	    return;
	}

	else if (is_name(arg2, (char*)"�anta"))
	{
      if (IS_SET(ch->quest,QUEST_BACKPACK))
      {
        do_tell_quest(ch,questman,(char*)"Bu e�yay� zaten alm��s�n. Kaybettiysen sigortadan faydalan.");
        return;
      }

	    if (ch->pcdata->questpoints >= 5000)
	    {
		ch->pcdata->questpoints -= 5000;
	        obj = create_object(get_obj_index(QUEST_ITEM4),ch->level);
	         SET_BIT(ch->quest,QUEST_BACKPACK);
	    }
	    else
	    {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
		      do_tell_quest(ch,questman,buf);
		return;
	    }
	}

	else if (is_name(arg2, (char*)"testi"))
	{
    if (IS_SET(ch->quest,QUEST_DECANTER))
    {
      do_tell_quest(ch,questman,(char*)"Bu e�yay� zaten alm��s�n. Kaybettiysen sigortadan faydalan.");
      return;
    }

	    if (ch->pcdata->questpoints >= 500)
	    {
		ch->pcdata->questpoints -= 500;
	        obj = create_object(get_obj_index(QUEST_ITEM5),ch->level);
	 	SET_BIT(ch->quest,QUEST_DECANTER);
	    }
	    else
	    {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }
	}

	else if (is_name(arg2, (char*)"kemer"))
	{
    if (IS_SET(ch->quest,QUEST_GIRTH))
    {
      do_tell_quest(ch,questman,(char*)"Bu e�yay� zaten alm��s�n. Kaybettiysen sigortadan faydalan.");
      return;
    }
	    if (ch->pcdata->questpoints >= 1000)
	    {
		ch->pcdata->questpoints -= 1000;
	        obj = create_object(get_obj_index(QUEST_ITEM1),ch->level);
	 	SET_BIT(ch->quest,QUEST_GIRTH);
	    }
	    else
	    {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }
	}
	else if (is_name(arg2, (char*)"i�lemeli"))
	{
    if (IS_SET(ch->quest,QUEST_YUZUK1))
    {
      do_tell_quest(ch,questman,(char*)"Bu e�yay� zaten alm��s�n. Kaybettiysen sigortadan faydalan.");
      return;
    }
	    if (ch->pcdata->questpoints >= 750)
	    {
		ch->pcdata->questpoints -= 750;
	        obj = create_object(get_obj_index(QUEST_ITEM_YUZUK1),ch->level);
          SET_BIT(ch->quest,QUEST_YUZUK1);
	    }
	    else
	    {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }
	}
  else if (is_name(arg2, (char*)"desenli"))
  {
    if (IS_SET(ch->quest,QUEST_YUZUK2))
    {
      do_tell_quest(ch,questman,(char*)"Bu e�yay� zaten alm��s�n. Kaybettiysen sigortadan faydalan.");
      return;
    }
      if (ch->pcdata->questpoints >= 750)
      {
    ch->pcdata->questpoints -= 750;
          obj = create_object(get_obj_index(QUEST_ITEM_YUZUK2),ch->level);
          SET_BIT(ch->quest,QUEST_YUZUK2);
      }
      else
      {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
    do_tell_quest(ch,questman,buf);
    return;
      }
  }
  else if (is_name(arg2, (char*)"oymal�"))
  {
    if (IS_SET(ch->quest,QUEST_YUZUK3))
    {
      do_tell_quest(ch,questman,(char*)"Bu e�yay� zaten alm��s�n. Kaybettiysen sigortadan faydalan.");
      return;
    }
      if (ch->pcdata->questpoints >= 750)
      {
    ch->pcdata->questpoints -= 750;
          obj = create_object(get_obj_index(QUEST_ITEM_YUZUK3),ch->level);
          SET_BIT(ch->quest,QUEST_YUZUK3);
      }
      else
      {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
    do_tell_quest(ch,questman,buf);
    return;
      }
  }
  else if (is_name(arg2, (char*)"kakmal�"))
  {
    if (IS_SET(ch->quest,QUEST_YUZUK4))
    {
      do_tell_quest(ch,questman,(char*)"Bu e�yay� zaten alm��s�n. Kaybettiysen sigortadan faydalan.");
      return;
    }
      if (ch->pcdata->questpoints >= 750)
      {
    ch->pcdata->questpoints -= 750;
          obj = create_object(get_obj_index(QUEST_ITEM_YUZUK4),ch->level);
          SET_BIT(ch->quest,QUEST_YUZUK4);
      }
      else
      {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
    do_tell_quest(ch,questman,buf);
    return;
      }
  }
  else if (is_name(arg2, (char*)"parlak"))
  {
    if (IS_SET(ch->quest,QUEST_SILAH1))
    {
      do_tell_quest(ch,questman,(char*)"Bu e�yay� zaten alm��s�n. Kaybettiysen sigortadan faydalan.");
      return;
    }
      if (ch->pcdata->questpoints >= 1000)
      {
    ch->pcdata->questpoints -= 1000;
          obj = create_object(get_obj_index(QUEST_ITEM_SILAH1),ch->level);
          SET_BIT(ch->quest,QUEST_SILAH1);
      }
      else
      {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
    do_tell_quest(ch,questman,buf);
    return;
      }
  }
  else if (is_name(arg2, (char*)"mat"))
  {
    if (IS_SET(ch->quest,QUEST_SILAH2))
    {
      do_tell_quest(ch,questman,(char*)"Bu e�yay� zaten alm��s�n. Kaybettiysen sigortadan faydalan.");
      return;
    }
      if (ch->pcdata->questpoints >= 1000)
      {
    ch->pcdata->questpoints -= 1000;
          obj = create_object(get_obj_index(QUEST_ITEM_SILAH2),ch->level);
          SET_BIT(ch->quest,QUEST_SILAH2);
      }
      else
      {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
    do_tell_quest(ch,questman,buf);
    return;
      }
  }
	else if (is_name(arg2, (char*)"pratik"))
	{
	    if (IS_SET(ch->quest, QUEST_PRACTICE))
	    {
        sprintf(buf, "�zg�n�m %s, bu �d�l� daha �nce alm��t�n!",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }

	    if (ch->pcdata->questpoints >= 1000)
	    {
		ch->pcdata->questpoints -= 1000;
	        ch->practice += 100;
          act("$N $e 100 pratik seans� veriyor.", ch, NULL, questman, TO_ROOM );
          act("$N sana 100 pratik seans� veriyor.",   ch, NULL, questman, TO_CHAR );
		SET_BIT(ch->quest, QUEST_PRACTICE);
	        return;
	    }
	    else
	    {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }
	}
	else if (is_name(arg2, (char*)"vampir"))
	{
	  if (ch->iclass != CLASS_VAMPIRE)
	  {
      sprintf(buf, "Sen bu yetene�i kazanamazs�n %s.",ch->name);
	    do_tell_quest(ch,questman,buf);
	    return;
	  }
	  if (ch->pcdata->questpoints >= 50)
	    {
		ch->pcdata->questpoints -= 50;
		sn = skill_lookup("vampire");
		ch->pcdata->learned[sn] = 100;
    act( "$N hortlakl���n s�rr�n� $e veriyor.", ch, NULL, questman, TO_ROOM );
  act( "$N sana hortlakl���n s�rr�n� veriyor.",   ch, NULL, questman, TO_CHAR );
act_color( "$CG�ky�z�nde �im�ekler �ak�yor.$c",   ch, NULL,
			questman, TO_ALL,POS_SLEEPING,CLR_BLUE );
	        return;
	    }
	    else
	    {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }
	}
	else if (is_name(arg2, (char*)"b�n b�nye"))
	{
	    if (ch->perm_stat[STAT_CON] >= get_max_train2(ch, STAT_CON))
	    {
        sprintf(buf, "�zg�n�m %s, b�nye niteli�in yeterince g��l�.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }

	    if (ch->pcdata->questpoints >= 250)
	    {
		ch->pcdata->questpoints -= 250;
		ch->perm_stat[STAT_CON] += 1;
	    }
	    else
	    {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }
	}
	else if (is_name(arg2, (char*)"samuray �l�m"))
	{
	    if (ch->iclass != CLASS_SAMURAI)
	    {
        sprintf(buf, "�zg�n�m %s, fakat sen bir samuray de�ilsin.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }

	    if (ch->pcdata->death < 1)
	    {
        sprintf(buf, "�zg�n�m %s, hen�z �lmemi�sin.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }

	    if (ch->pcdata->questpoints >= 50)
	    {
		ch->pcdata->questpoints -= 50;
		ch->pcdata->death -= 1;
	    }
	    else
	    {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }
	}
	else if (is_name(arg2, (char*)"katana"))
	{
	  AFFECT_DATA af;
	  OBJ_DATA *katana;
	    if (ch->iclass != 9)
	    {
        sprintf(buf, "�zg�n�m %s, fakat sen bir samuray de�ilsin.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }

	    if ( (katana = get_obj_list(ch,(char*)"katana",ch->carrying)) == NULL)
	    {
        sprintf(buf, "�zg�n�m %s, fakat katanan yan�nda de�il.",ch->name);
		 do_tell_quest(ch,questman,buf);
	         return;
	    }

	    if (IS_WEAPON_STAT(katana,WEAPON_KATANA))
	    {
        sprintf(buf, "�zg�n�m %s, fakat katanan ilk g�revi ge�mi� zaten.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }

	    if (ch->pcdata->questpoints >= 100)
	    {
		ch->pcdata->questpoints -= 100;
		af.where	= TO_WEAPON;
		af.type		= gsn_reserved;
		af.level	= 100;
		af.duration	= -1;
		af.modifier	= 0;
		af.bitvector	= WEAPON_KATANA;
		af.location	= APPLY_NONE;
		affect_to_obj(katana,&af);
    sprintf(buf, "Katanandaki g�c�n giderek artaca��n� hissediyorsun.");
		do_tell_quest(ch,questman,buf);
	    }
	    else
	    {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }
	}
	else if (is_name(arg2, (char*)"keskin ikinci"))
	{
	  AFFECT_DATA af;
	  OBJ_DATA *katana;

	    if (ch->iclass != 9)
	    {
        sprintf(buf, "�zg�n�m %s, fakat sen bir samuray de�ilsin.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }

	    if ( (katana = get_obj_list(ch,(char*)"katana",ch->carrying)) == NULL)
	    {
        sprintf(buf, "�zg�n�m %s, fakat katanan yan�nda de�il.",ch->name);
		 do_tell_quest(ch,questman,buf);
	         return;
	    }

	    if (!IS_WEAPON_STAT(katana,WEAPON_KATANA))
	    {
        sprintf(buf, "�zg�n�m %s, fakat katanan ilk g�revi hen�z ge�memi�.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }

	    if (ch->pcdata->questpoints >= 100)
	    {
		ch->pcdata->questpoints -= 100;
		af.where	= TO_WEAPON;
		af.type		= gsn_reserved;
		af.level	= 100;
		af.duration	= -1;
		af.modifier	= 0;
		af.bitvector	= WEAPON_SHARP;
		af.location	= APPLY_NONE;
		affect_to_obj(katana,&af);
    sprintf(buf, "�u andan sonra katanan en keskin k�l��lardan daha keskin olacak.");
		do_tell_quest(ch,questman,buf);
	    }
	    else
	    {
        sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }
	}
	else if (is_name(arg2, (char*)"alt�n"))
	{
	    if (ch->pcdata->questpoints >= 500)
	    {
		ch->pcdata->questpoints -= 500;
	        ch->gold += 350000;
          act("$N $e 350.000 alt�n veriyor.", ch, NULL, questman, TO_ROOM );
          act( "$N sana 350.000 alt�n veriyor.",   ch, NULL, questman, TO_CHAR );
	        return;
	    }
	    else
	    {
		sprintf(buf, "�zg�n�m %s, bunun i�in yeterli g�rev puan�n yok.",ch->name);
		do_tell_quest(ch,questman,buf);
		return;
	    }
	}
	else
	{
    sprintf(buf, "Ondan bende yok, %s.",ch->name);
	    do_tell_quest(ch,questman,buf);
	}
	if (obj != NULL)
	{
	 if ( obj->pIndexData->vnum == QUEST_ITEM4
	      || obj->pIndexData->vnum == QUEST_ITEM5)
	 {
	  sprintf( buf,obj->pIndexData->extra_descr->description,ch->name );
	  obj->extra_descr = new_extra_descr();
	  obj->extra_descr->keyword =
		str_dup( obj->pIndexData->extra_descr->keyword );
	  obj->extra_descr->description = str_dup( buf );
	  obj->extra_descr->next = NULL;
	 }
   if ( obj->pIndexData->vnum == QUEST_ITEM1 || obj->pIndexData->vnum == QUEST_ITEM_YUZUK1
	      || obj->pIndexData->vnum == QUEST_ITEM_YUZUK2 || obj->pIndexData->vnum == QUEST_ITEM_YUZUK3
        || obj->pIndexData->vnum == QUEST_ITEM_YUZUK4 || obj->pIndexData->vnum == QUEST_ITEM_SILAH1
        || obj->pIndexData->vnum == QUEST_ITEM_SILAH2
      )
	 {
	  sprintf( buf, obj->short_descr,
		IS_GOOD(ch) ? "parlak" :
		IS_NEUTRAL(ch) ? "mavi-ye�il" : "k�z�l", ch->name);
	  free_string( obj->short_descr );
	  obj->short_descr = str_dup( buf );
	 }
   act("$N $e $p veriyor.", ch, obj, questman, TO_ROOM );
   act("$N sana $p veriyor.",   ch, obj, questman, TO_CHAR );
	 obj_to_char(obj, ch);
	}
	return;
    }
    else if (!strcmp(arg1, "iste"))
    {
      act("$n $Z g�rev istiyor.", ch, NULL, questman, TO_ROOM);
act ("$Z g�rev istiyorsun.",ch, NULL, questman, TO_CHAR);

	if (IS_SET(ch->act, PLR_QUESTOR))
	{
    sprintf(buf, "Zaten g�revdesin. �nanm�yorsan \"g�rev bilgi\" yaz!");
	    do_tell_quest(ch,questman,buf);
	    return;
	}
	if (ch->pcdata->nextquest > 0)
	{
    sprintf(buf, "�ok cesursun %s, fakat izin ver ba�kalar� da nasiplensin.",ch->name);
    do_tell_quest(ch,questman,buf);
    sprintf(buf, "Daha sonra tekrar u�ra.");
	    do_tell_quest(ch,questman,buf);
	    return;
	}

  sprintf(buf, "Te�ekk�r ederim, cesur %s!",ch->name);
	do_tell_quest(ch,questman,buf);

	generate_quest(ch, questman);

        if (ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0)
	{
            ch->pcdata->countdown = number_range(15,30);
	    SET_BIT(ch->act, PLR_QUESTOR);
      sprintf(buf, "Bu g�revi tamamlamak i�in %d dakikan var.",ch->pcdata->countdown);
	    do_tell_quest(ch,questman,buf);
      sprintf(buf, "Tanr�lar seninle olsun!");
	    do_tell_quest(ch,questman,buf);
	}
	return;
    }
    else if (!strcmp(arg1, "iptal"))

    {

      if (!IS_SET(ch->act, PLR_QUESTOR))

      {

        sprintf(buf, "G�revde de�ilsin.");
        do_tell_quest(ch, questman, buf);

        return;

      }

      REMOVE_BIT(ch->act, PLR_QUESTOR);
      ch->pcdata->questgiver = 0;
      ch->pcdata->countdown = 0;
      ch->pcdata->nextquest = 5;

      ch->pcdata->questmob = 0;

      ch->pcdata->questobj = 0;
      ch->pcdata->questroom = 0;



      sprintf(buf, "G�revini iptal ettim.");

      do_tell_quest(ch, questman, buf);

      return;

    }

	else if (!strcmp(arg1, "bitti"))
    {
      act("$n $E g�revi bitirdi�ini haber veriyor.", ch, NULL, questman, TO_ROOM);
act ("$E g�revi bitirdi�ini haber veriyorsun.",ch, NULL, questman, TO_CHAR);
	if (ch->pcdata->questgiver != questman->pIndexData->vnum)
	{
    sprintf(buf, "Seni bir g�reve g�nderdi�imi hat�rlam�yorum!");
	    do_tell_quest(ch,questman,buf);
	    return;
	}

	if (IS_SET(ch->act, PLR_QUESTOR))
	{
	    if (ch->pcdata->questmob == -1 && ch->pcdata->countdown > 0)
	    {
		int reward=0, pointreward=0, pracreward=0,level;

		level = ch->level;
		reward = 100 + dice( level, 20);
		reward = UMAX( 180 , reward );
		pointreward = number_range(20,40);

		if(IS_SET(ch->pcdata->dilek,DILEK_FLAG_GOREV))
		{
				printf_to_char( ch , "{CG�rev dile�in sayesinde kazand���n GP art�yor.{x\n\r" );
				pointreward *= 2;
		}

    sprintf(buf, "Tebrikler!");
    do_tell_quest(ch,questman,buf);
    sprintf(buf,"Kar��l���nda sana %d GP ve %d alt�n veriyorum.",pointreward,reward);
		do_tell_quest(ch,questman,buf);
		if (chance(2))
		{
		    pracreward = number_range(1,6);
        sprintf(buf, "%d pratik seans� kazand�n!\n\r",pracreward);
		    send_to_char(buf, ch);
		    ch->practice += pracreward;
		}

	        REMOVE_BIT(ch->act, PLR_QUESTOR);
	        ch->pcdata->questgiver = 0;
	        ch->pcdata->countdown = 0;
	        ch->pcdata->questmob = 0;
		ch->pcdata->questobj = 0;
	        ch->pcdata->nextquest = 5;
		ch->gold += reward;
		ch->pcdata->questpoints += pointreward;

	        return;
	    }
	    else if (ch->pcdata->questobj > 0 && ch->pcdata->countdown > 0)
	    {
		bool obj_found = FALSE;

    		for (obj = ch->carrying; obj != NULL; obj= obj_next)
    		{
        	    obj_next = obj->next_content;

	    if (obj != NULL && obj->pIndexData->vnum == ch->pcdata->questobj
		&& strstr( obj->extra_descr->description, ch->name ) != NULL )
		    {
			obj_found = TRUE;
            	        break;
		    }
        	}
		if (obj_found == TRUE)
		{
		    int reward, pointreward, pracreward;

		    reward = 200 + number_range(1, 20 * ch->level);
		    pointreward = number_range(15,40);

			if(IS_SET(ch->pcdata->dilek,DILEK_FLAG_GOREV))
			{
				printf_to_char( ch , "{CG�rev dile�in sayesinde kazand���n GP art�yor.{x\n\r" );
				pointreward *= 2;
			}

        act("$p objesini $E veriyorsun.",ch, obj, questman, TO_CHAR);
				act("$n $p objesini $E veriyor.",ch, obj, questman, TO_ROOM);

        sprintf(buf, "Tebrikler!");
    do_tell_quest(ch,questman,buf);
    sprintf(buf,"Kar��l���nda sana %d GP ve %d alt�n veriyorum.",pointreward,reward);
		    do_tell_quest(ch,questman,buf);
		    if (chance(15))
		    {
		        pracreward = number_range(1,6);
            sprintf(buf, "%d pratik seans� kazand�n!\n\r",pracreward);
		        send_to_char(buf, ch);
		        ch->practice += pracreward;
		    }

	            REMOVE_BIT(ch->act, PLR_QUESTOR);
	            ch->pcdata->questgiver = 0;
	            ch->pcdata->countdown = 0;
	            ch->pcdata->questmob = 0;
		    ch->pcdata->questobj = 0;
	            ch->pcdata->nextquest = 5;
		    ch->gold += reward;
		    ch->pcdata->questpoints += pointreward;
		    extract_obj(obj);
		    return;
		}
		else
		{
      sprintf(buf, "Hen�z g�revi bitirmedin. Fakat hala zaman�n var!");
		    do_tell_quest(ch,questman,buf);
		    return;
		}
		return;
	    }
	    else if ((ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0) && ch->pcdata->countdown > 0)
	    {
        sprintf(buf, "Hen�z g�revi bitirmedin. Fakat hala zaman�n var!");
		do_tell_quest(ch,questman,buf);
		return;
	    }
	}
	if (ch->pcdata->nextquest > 0)
  sprintf(buf,"Maalesef g�revi zaman�nda tamamlayamad�n!");
else sprintf(buf, "�nce bir g�rev �STEmelisin, %s.",ch->name);
	do_tell_quest(ch,questman,buf);
	return;
    }
    else if (!strcmp(arg1, "sigorta"))
    {
	if (arg2[0] == '\0')
	{
    send_to_char("G�rev objesinin sigortas�ndan faydalanmak i�in 'g�rev sigorta <obje>' yaz�n.\n\r",ch);
	    return;
	}

	if (is_name(arg2, (char*)"kemer"))
	{
	  if (IS_SET(ch->quest,QUEST_GIRTH))
	    {
		trouble_vnum = QUEST_ITEM1;
	    }
	}
	else if (is_name(arg2, (char*)"�anta"))
	{
	  if (IS_SET(ch->quest,QUEST_BACKPACK))
	    {
		trouble_vnum = QUEST_ITEM4;
	    }
	}
	else if (is_name(arg2, (char*)"testi"))
	{
	  if (IS_SET(ch->quest,QUEST_DECANTER))
	    {
		trouble_vnum = QUEST_ITEM5;
	    }
	}
	else if (is_name(arg2, (char*)"parlak"))
	{
	  if (IS_SET(ch->quest,QUEST_SILAH1))
	    {
		trouble_vnum = QUEST_ITEM_SILAH1;
	    }
	}
  else if (is_name(arg2, (char*)"mat"))
	{
	  if (IS_SET(ch->quest,QUEST_SILAH2))
	    {
		trouble_vnum = QUEST_ITEM_SILAH2;
	    }
	}
  else if (is_name(arg2, (char*)"i�lemeli"))
	{
	  if (IS_SET(ch->quest,QUEST_YUZUK1))
	    {
		trouble_vnum = QUEST_ITEM_YUZUK1;
	    }
	}
  else if (is_name(arg2, (char*)"desenli"))
	{
	  if (IS_SET(ch->quest,QUEST_YUZUK2))
	    {
		trouble_vnum = QUEST_ITEM_YUZUK2;
	    }
	}
  else if (is_name(arg2, (char*)"oymal�"))
	{
	  if (IS_SET(ch->quest,QUEST_YUZUK3))
	    {
		trouble_vnum = QUEST_ITEM_YUZUK3;
	    }
	}
  else if (is_name(arg2, (char*)"kakmal�"))
	{
	  if (IS_SET(ch->quest,QUEST_YUZUK4))
	    {
		trouble_vnum = QUEST_ITEM_YUZUK4;
	    }
	}
	if (!trouble_vnum)
	{
    sprintf(buf,"�zg�n�m %s, fakat bahsetti�in �d�l� hen�z almam��s�n.\n\r",ch->name);
	    do_tell_quest(ch,questman,buf);
	    return;
	}

        for(obj = object_list; obj != NULL; obj = obj_next)
	{
	 obj_next = obj->next;
	 if ( obj->pIndexData->vnum == trouble_vnum &&
		strstr( obj->short_descr, ch->name))
	 {
	  extract_obj( obj );
	  break;
	 }
	}
        obj = create_object(get_obj_index(trouble_vnum),ch->level);
	if ( obj->pIndexData->vnum == QUEST_ITEM4
	      || obj->pIndexData->vnum == QUEST_ITEM5)
	 {
	  sprintf( buf,obj->pIndexData->extra_descr->description,ch->name );
	  obj->extra_descr = new_extra_descr();
	  obj->extra_descr->keyword =
		str_dup( obj->pIndexData->extra_descr->keyword );
	  obj->extra_descr->description = str_dup( buf );
	  obj->extra_descr->next = NULL;
	 }
	 if ( obj->pIndexData->vnum == QUEST_ITEM1 || obj->pIndexData->vnum == QUEST_ITEM_YUZUK1
	      || obj->pIndexData->vnum == QUEST_ITEM_YUZUK2 || obj->pIndexData->vnum == QUEST_ITEM_YUZUK3
        || obj->pIndexData->vnum == QUEST_ITEM_YUZUK4 || obj->pIndexData->vnum == QUEST_ITEM_SILAH1
        || obj->pIndexData->vnum == QUEST_ITEM_SILAH2
      )
	 {
	  sprintf( buf, obj->short_descr,
      IS_GOOD(ch) ? "parlak" :
  		IS_NEUTRAL(ch) ? "mavi-ye�il" : "k�z�l", ch->name);
	  free_string( obj->short_descr );
	  obj->short_descr = str_dup( buf );
	 }
   act( "$N $p objesini $e veriyor.", ch, obj, questman, TO_ROOM );
   act( "$N $p objesini sana veriyor.",   ch, obj, questman, TO_CHAR );
	obj_to_char(obj, ch);
     return;
    }

    send_to_char("G�REV KOMUTLARI: puan bilgi zaman iste bitti iptal liste �zellik sat�nal sigorta.\n\r",ch);
    send_to_char("Daha fazla bilgi i�in: yard�m g�rev.\n\r",ch);
    return;
}

void generate_quest(CHAR_DATA *ch, CHAR_DATA *questman)
{
    CHAR_DATA *victim;
    MOB_INDEX_DATA *vsearch;
    ROOM_INDEX_DATA *room;
    OBJ_DATA *eyed;
    char buf [MAX_STRING_LENGTH];
    int level_diff, i;
    int mob_buf[300],mob_count;
    int found;

    room	=	(ROOM_INDEX_DATA *)alloc_perm( sizeof (*room));

    mob_count = 0;
    for ( i=0; i< MAX_KEY_HASH; i++)
    {
     if ((vsearch  = mob_index_hash[i]) == NULL) continue;
     level_diff = vsearch->level - ch->level;
     if ( (ch->level < 51 && (level_diff > 4 || level_diff < -1))
	   || (ch->level > 50 && (level_diff > 6 || level_diff < 0))
	   || vsearch->pShop != NULL
    	   || IS_SET(vsearch->act,ACT_TRAIN)
    	   || IS_SET(vsearch->act,ACT_PRACTICE)
    	   || IS_SET(vsearch->act,ACT_IS_HEALER)
    	   || IS_SET(vsearch->act,ACT_NOTRACK))
	continue;
     mob_buf[mob_count] = vsearch->vnum;
     mob_count++;
     if (mob_count > 299) break;
    }

    if (chance(40))
    {
       int objvnum = 0;
       int i;

       if (mob_count > 0)
       {
	 found = number_range(0,mob_count-1);
	 for( i=0; i< mob_count; i++)
	 {
	    if ((vsearch = get_mob_index( mob_buf[found] )) == NULL )
	    {
		bug("Unknown mob in generate_obj_quest: %d",mob_buf[found]);
		found++;
		if ( found > (mob_count-1) ) break;
		else continue;
	    }
	    else break;
    	 }
       }
       else vsearch = NULL;

       if ( vsearch == NULL || ( victim = get_quest_world( ch, vsearch ) ) == NULL )
       {
	sprintf(buf, "�zg�n�m ama �u an sana verebilece�im bir g�rev yok.");
	do_tell_quest(ch,questman,buf);
	sprintf(buf, "Daha sonra tekrar dene.");
	do_tell_quest(ch,questman,buf);
	ch->pcdata->nextquest = 5;
        return;
       }

       if ( (room = victim->in_room) == NULL )
       {
         sprintf(buf, "�zg�n�m ama �u an sana verebilece�im bir g�rev yok.");
       	do_tell_quest(ch,questman,buf);
       	sprintf(buf, "Daha sonra tekrar dene.");
	do_tell_quest(ch,questman,buf);
	ch->pcdata->nextquest = 5;
        return;
       }

	switch(number_range(0,3))
	{
	    case 0:
	    objvnum = QUEST_OBJQUEST1;
	    break;

	    case 1:
	    objvnum = QUEST_OBJQUEST2;
	    break;

	    case 2:
	    objvnum = QUEST_OBJQUEST3;
	    break;

	    case 3:
	    objvnum = QUEST_OBJQUEST4;
	    break;

	}


	if (IS_GOOD(ch))
		i=0;
	else if (IS_EVIL(ch))
		i=2;
	else i = 1;

        eyed = create_object( get_obj_index(objvnum), ch->level );
	eyed->owner = str_dup(ch->name);
	eyed->from = str_dup(ch->name);
	eyed->altar = hometown_table[ch->hometown].altar[i];
	eyed->pit = hometown_table[ch->hometown].pit[i];
	eyed->level = ch->level;

	sprintf( buf, eyed->description, ch->name	 );
	free_string( eyed->description );
	eyed->description = str_dup( buf );

        sprintf( buf, eyed->pIndexData->extra_descr->description, ch->name );
	eyed->extra_descr = new_extra_descr();
	eyed->extra_descr->keyword =
		str_dup( eyed->pIndexData->extra_descr->keyword );
	eyed->extra_descr->description = str_dup( buf );
	eyed->extra_descr->next = NULL;

	eyed->cost = 0;
	eyed->timer = 30;

	obj_to_room(eyed, room);
	ch->pcdata->questobj = eyed->pIndexData->vnum;
  ch->pcdata->questroom = room->vnum;

  sprintf(buf, "%s hazine dairesinden �al�nd�!",eyed->short_descr);
	do_tell_quest(ch,questman,buf);
	do_tell_quest(ch,questman, (char*)"Saray b�y�c�s� sihirli aynas�yla yerini belirledi.");

	/* I changed my area names so that they have just the name of the area
	   and none of the level stuff. You may want to comment these next two
	   lines. - Vassago */

     sprintf(buf, "%s b�lgesindeki %s isimli yere gitmelisin!",room->area->name, room->name);
	do_tell_quest(ch,questman,buf);
	return;
    }

    /* Quest to kill a mob */

    else
    {
     if (mob_count > 0)
     {
	found = number_range(0,mob_count-1);
	for( i=0; i< mob_count; i++)
	{
	   if ((vsearch = get_mob_index( mob_buf[found] )) == NULL
	      || (IS_EVIL(vsearch) && IS_EVIL(ch))
	      || (IS_GOOD(vsearch) && IS_GOOD(ch))
	      || (IS_NEUTRAL(vsearch) && IS_NEUTRAL(ch)) )
		{
		 if (vsearch == NULL)
			bug("Unknown mob in mob_quest: %d",mob_buf[found]);
		 found++;
		 if ( found > (mob_count-1) )
		 {
		   vsearch = NULL;
		   break;
		 }
		 else continue;
		}
	   else break;
    	}
     }
     else vsearch = NULL;

     if ( vsearch == NULL
	 || (victim = get_quest_world(ch, vsearch)) == NULL
	 || (room = victim->in_room ) == NULL
	 || IS_SET(room->area->area_flag,AREA_HOMETOWN))
     {
       sprintf(buf, "�zg�n�m ama �u an sana verebilece�im bir g�rev yok.");
      do_tell_quest(ch,questman,buf);
      sprintf(buf, "Daha sonra tekrar dene.");
	do_tell_quest(ch,questman,buf);
	ch->pcdata->nextquest = 5;
        return;
     }

    if (IS_GOOD(ch))
       {
         sprintf(buf, "Diyar�n az�l� asilerinden %s,	zindandan ka�t�!",victim->short_descr);
	do_tell_quest(ch,questman,buf);
  sprintf(buf, "Ka����ndan bu yana tahminimizce %d sivili katletti!", number_range(2,20));
	do_tell_quest(ch,questman,buf);
	do_tell_quest(ch,questman,(char*)"The penalty for this crime is death, and you are to deliver the sentence!");
        }
    else
       {
         sprintf(buf, "�ahsi d��man�m %s, kraliyet tac�na kar�� tehdit olu�turuyor.",victim->short_descr);
   			do_tell_quest(ch,questman,buf);
   			sprintf(buf, "Bu tehdit yokedilmeli!");
        do_tell_quest(ch,questman,buf);
       }

     if (room->name != NULL)
     {
       sprintf(buf, "%s �u s�ralar %s b�lgesindedir!",victim->short_descr,room->area->name);
        do_tell_quest(ch,questman,buf);

	/* I changed my area names so that they have just the name of the area
	   and none of the level stuff. You may want to comment these next two
	   lines. - Vassago */

	sprintf(buf, "Yeri %s civar�nda.",room->area->name);
	do_tell_quest(ch,questman,buf);
     }
     ch->pcdata->questmob = victim->pIndexData->vnum;
     ch->pcdata->questroom = room->vnum;
    }
    return;
}

/* Called from update_handler() by pulse_area */

void quest_update(void)
{
    CHAR_DATA *ch, *ch_next;

    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;

	if (IS_NPC(ch)) continue;

	if (ch->pcdata->nextquest > 0)
	{
	    ch->pcdata->nextquest--;

	    if (ch->pcdata->nextquest == 0)
	    {
        send_to_char("Yeniden g�rev isteyebilirsin.\n\r",ch);
	        continue;
	    }
	}
        else if (IS_SET(ch->act,PLR_QUESTOR))
        {
	    if (--ch->pcdata->countdown <= 0)
	    {

	        ch->pcdata->nextquest = 0;
          send_to_char("G�rev s�ren doldu!\n\rYeni bir g�rev isteyebilirsin.\n\r", ch);
	        REMOVE_BIT(ch->act, PLR_QUESTOR);

                ch->pcdata->questgiver = 0;
                ch->pcdata->countdown = 0;
                ch->pcdata->questmob = 0;
                ch->pcdata->questroom = 0;
		            ch->pcdata->questobj = 0;
	    }
	    if (ch->pcdata->countdown > 0 && ch->pcdata->countdown < 6)
	    {
        send_to_char("Acele et, g�rev s�ren dolmak �zere!\n\r",ch);
	        continue;
	    }
        }
    }
    return;
}

void do_tell_quest( CHAR_DATA *ch, CHAR_DATA *victim, char *argument )
{

	    act_color("$N: $C$t$c",ch,argument,victim,TO_CHAR,POS_DEAD, CLR_MAGENTA_BOLD );

		return;
}

CHAR_DATA *get_quest_world( CHAR_DATA *ch, MOB_INDEX_DATA *victim)
{
    CHAR_DATA *wch;

    for ( wch = char_list; wch != NULL ; wch = wch->next )
    {
        if ( wch->in_room == NULL
		|| wch->pIndexData != victim)
            continue;

	return wch;
    }

    return NULL;
}

void do_eniyi(CHAR_DATA *ch,char *argument)
{
	CHAR_DATA *questman;
	char buf[MAX_STRING_LENGTH];
	int bolge=0,seviye=0,zz1zz=0,zz2zz=0,zz3zz=0,zararzari=0,ac1ac=0,ac2ac=0,ac3ac=0,actoplam=0;
	char arg1 [MAX_INPUT_LENGTH];
	OBJ_DATA *obj,*zz1,*zz2,*zz3,*ac1,*ac2,*ac3;
	AFFECT_DATA *paf;

	for ( questman = ch->in_room->people; questman != NULL; questman = questman->next_in_room )
    {
	if (!IS_NPC(questman)) continue;
        if (questman->spec_fun == spec_lookup( (char*)"spec_questmaster" )) break;
    }

    if (questman == NULL || questman->spec_fun != spec_lookup( (char*)"spec_questmaster" ))
    {
        send_to_char("Odada bu i�leri yapan bir g�revci g�remiyorum.\n\r", ch);
        return;
    }

if ( argument[0] == '\0' )
	{
		send_to_char("Hangi b�lgene giyece�in ekipmanlar hakk�nda bilgi istiyorsun?\n\r",ch);
		return;
	}
	argument = one_argument(argument, arg1);

	seviye = ch->level;
	bolge=-1;
	if(!strcmp(arg1,"parmak"))
		bolge=ITEM_WEAR_FINGER;
	else if(!strcmp(arg1,"boyun"))
		bolge=ITEM_WEAR_NECK;
	else if(!strcmp(arg1,"g�vde"))
		bolge=ITEM_WEAR_BODY;
	else if(!strcmp(arg1,"kafa"))
		bolge=ITEM_WEAR_HEAD;
	else if(!strcmp(arg1,"bacaklar"))
		bolge=ITEM_WEAR_LEGS;
	else if(!strcmp(arg1,"ayaklar"))
		bolge=ITEM_WEAR_FEET;
	else if(!strcmp(arg1,"eller"))
		bolge=ITEM_WEAR_HANDS;
	else if(!strcmp(arg1,"kollar"))
		bolge=ITEM_WEAR_ARMS;
	else if(!strcmp(arg1,"v�cut"))
		bolge=ITEM_WEAR_ABOUT;
	else if(!strcmp(arg1,"bel"))
		bolge=ITEM_WEAR_WAIST;
	else if(!strcmp(arg1,"bilek"))
		bolge=ITEM_WEAR_WRIST;
	else if(!strcmp(arg1,"s�z�len"))
		bolge=ITEM_WEAR_FLOAT;
	else
	{
		send_to_char("V�cudunda b�yle bir b�lge g�remiyorum!\n\r",ch);
		return;
	}
	if(ch->gold<((ch->level/2)+1))
	{
		send_to_char("Yeterli paran yok, bilgi veremem.\n\r",ch);
		return;
	}

	zz1=NULL;
	zz2=NULL;
	zz3=NULL;
	ac1=NULL;
	ac2=NULL;
	ac3=NULL;

	act("$n $Z ekipman bilgisi istiyor.", ch, NULL, questman, TO_ROOM);

	for( obj=object_list; obj!=NULL; obj = obj->next )
	{
		zararzari=0;
		actoplam=0;
		if ( CAN_WEAR(obj,ITEM_TAKE) &&
			 CAN_WEAR( obj, bolge ) &&
			 obj->level <= seviye)
		{
			if (!obj->enchanted)
			{
				for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
				{
					if ( paf->location != APPLY_NONE && paf->modifier != 0 )
					{
						if( (paf->location==APPLY_DAMROLL) || (paf->location==APPLY_HITROLL) )
						{
							zararzari += paf->modifier;
						}
					}
				}//for
				if( (zararzari>=zz1zz) )
				{
					zz3=zz2;
					zz2=zz1;
					zz1=obj;
					zz1zz=zararzari;
				}
				else if( (zararzari>=zz2zz) )
				{
					zz3=zz2;
					zz2=obj;
					zz2zz=zararzari;
				}
				else if( (zararzari>=zz3zz) )
				{
					zz3=obj;
					zz3zz=zararzari;
				}

			}//if (!obj->enchanted)
			if(obj->item_type==ITEM_ARMOR)
			{
				actoplam = obj->value[0] + obj->value[1] + obj->value[2] + obj->value[3];
				if( (actoplam>=ac1ac))
				{
					ac3=ac2;
					ac2=ac1;
					ac1=obj;
					ac1ac=actoplam;
				}
				else if ( (actoplam>=ac2ac) )
				{
					ac3=ac2;
					ac2=obj;
					ac2ac=actoplam;
				}
				else if( (actoplam>=ac3ac))
				{
					ac3=obj;
					ac3ac=actoplam;
				}
			}
		}//if can_wear
	}//for
	if(zz1== NULL && ac1==NULL)
	{
		do_tell_quest(ch,questman,(char*)"�u an bir�ey hat�rlayam�yorum. San�r�m ya�lan�yorum.");
		do_tell_quest(ch,questman,(char*)"Daha sonra tekrar u�ra l�tfen.");
		return;
	}
	ch->gold -= (ch->level/2)+1;
	do_tell_quest(ch,questman,(char*)"Bir d���neyim... Evet san�r�m bir�eyler hat�rlad�m.");
	do_tell_quest(ch,questman,(char*)"Baz� ekipmanlar hat�rl�yorum, senin giyebilece�in seviyede ekipmanlar.");

	if(zz1 != NULL)
	{
		do_tell_quest(ch,questman,(char*)"Vuru�lar�n�n g�c�n� ve isabetini art�racak ekipmanlar. Mesela...");
		sprintf(buf,"%s",zz1->short_descr);
		do_tell_quest(ch,questman,(char*)buf);
	}
	if(zz2 != NULL)
	{
		sprintf(buf,"%s",zz2->short_descr);
		do_tell_quest(ch,questman,(char*)buf);
	}
	if(zz3 != NULL)
	{
		sprintf(buf,"%s",zz3->short_descr);
		do_tell_quest(ch,questman,(char*)buf);
	}
	if(ac1 != NULL)
	{
		do_tell_quest(ch,questman,(char*)"Bir de seni koruyacak ekipmanlar var akl�ma gelen. Mesela...");
		sprintf(buf,"%s",ac1->short_descr);
		do_tell_quest(ch,questman,(char*)buf);
	}
	if(ac2 != NULL)
	{
		sprintf(buf,"%s",ac2->short_descr);
		do_tell_quest(ch,questman,(char*)buf);
	}
	if(ac3 != NULL)
	{
		sprintf(buf,"%s",ac3->short_descr);
		do_tell_quest(ch,questman,(char*)buf);
	}
}
