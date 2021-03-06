/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "titanic/npcs/doorbot.h"
#include "titanic/core/room_item.h"

namespace Titanic {

BEGIN_MESSAGE_MAP(CDoorbot, CTrueTalkNPC)
	ON_MESSAGE(MovieEndMsg)
	ON_MESSAGE(OnSummonBotMsg)
	ON_MESSAGE(TrueTalkTriggerActionMsg)
	ON_MESSAGE(DoorbotNeededInHomeMsg)
	ON_MESSAGE(DoorbotNeededInElevatorMsg)
	ON_MESSAGE(LeaveViewMsg)
	ON_MESSAGE(TimerMsg)
	ON_MESSAGE(NPCPlayTalkingAnimationMsg)
	ON_MESSAGE(NPCPlayIdleAnimationMsg)
	ON_MESSAGE(PutBotBackInHisBoxMsg)
	ON_MESSAGE(DismissBotMsg)
	ON_MESSAGE(MovieFrameMsg)
	ON_MESSAGE(TrueTalkNotifySpeechEndedMsg)
	ON_MESSAGE(TextInputMsg)
	ON_MESSAGE(EnterViewMsg)
	ON_MESSAGE(ActMsg)
END_MESSAGE_MAP()

int CDoorbot::_v1;
int CDoorbot::_v2;

CDoorbot::CDoorbot() : CTrueTalkNPC() {
	_field108 = 0;
	_timerId = 0;
	_field110 = 0;
	_field114 = 0;
}

void CDoorbot::save(SimpleFile *file, int indent) {
	file->writeNumberLine(1, indent);
	file->writeNumberLine(_v1, indent);
	file->writeNumberLine(_v2, indent);

	file->writeNumberLine(_field108, indent);
	file->writeNumberLine(_timerId, indent);
	file->writeNumberLine(_field110, indent);
	file->writeNumberLine(_field114, indent);

	CTrueTalkNPC::save(file, indent);
}

void CDoorbot::load(SimpleFile *file) {
	file->readNumber();
	_v1 = file->readNumber();
	_v2 = file->readNumber();

	_field108 = file->readNumber();
	_timerId = file->readNumber();
	_field110 = file->readNumber();
	_field114 = file->readNumber();

	CTrueTalkNPC::load(file);
}

bool CDoorbot::MovieEndMsg(CMovieEndMsg *msg) {
	if (_npcFlags & NPCFLAG_8000000) {
		switch (_field108) {
		case 3:
			startTalking(this, 221482);
			_field108 = 4;
			break;

		case 6:
			if (clipExistsByEnd("Cloak On", msg->_endFrame)) {
				petShow();
				setState1C(true);
				changeView("ServiceElevator.Node 1.S");
				changeView("ServiceElevator.Node 1.N");
			}
			break;

		case 7:
			startTalking(this, 221467);
			_field108 = 8;
			break;

		case 9:
			startTalking(this, 221468);
			break;

		case 11:
			changeView("ServiceElevator.Node 1.S");
			changeView("MoonEmbLobby.Node 1.NE");
			break;

		default:
			break;
		}

		CTrueTalkNPC::MovieEndMsg(msg);
	} else if (_npcFlags & NPCFLAG_100000) {
		if (clipExistsByEnd("Cloak Off", msg->_endFrame)) {
			_npcFlags = (_npcFlags & ~NPCFLAG_8) | NPCFLAG_4;
			endTalking(this, false);
			startTalking(this, 221474);
			_npcFlags &= ~NPCFLAG_8000000;
			_field108 = 0;
		} else if (clipExistsByEnd("Cloak On", msg->_endFrame)) {
			petShow();
			setState1C(true);
			changeView("ServiceElevator.Node 1.S");
		} else {
			CTrueTalkNPC::MovieEndMsg(msg);
		}
	} else if (_npcFlags & NPCFLAG_400000) {
		if (clipExistsByEnd("Whizz On Left", msg->_endFrame)
				|| clipExistsByEnd("Whizz On Right", msg->_endFrame)) {
			setPosition(Point((600 - _bounds.width()) / 2 + 18, 42));
			loadFrame(0);
			endTalking(this, true);
			_npcFlags |= NPCFLAG_4;
			petSetArea(PET_CONVERSATION);
		} else if (clipExistsByEnd("Whizz Off Left", msg->_endFrame)
				|| clipExistsByEnd("Whizz Off Right", msg->_endFrame)) {
			CPutBotBackInHisBoxMsg boxMsg;
			boxMsg.execute(this);
			if (_npcFlags & NPCFLAG_4000000)
				startAnimTimer("SummonBellbot", 1500);
		} else {
			CTrueTalkNPC::MovieEndMsg(msg);
		}
	} else {
		CTrueTalkNPC::MovieEndMsg(msg);
	}

	return true;
}

bool CDoorbot::OnSummonBotMsg(COnSummonBotMsg *msg) {
	const char *const ROOM_WAVES[8][2] = {
		{ "EmbLobby", "z#186.wav" },
		{ "PromenadeDeck", "z#184.wav" },
		{ "Arboretum", "z#188.wav" },
		{ "Frozen Arboretum", "z#188.wav" },
		{ "Bar", "z#187.wav" },
		{ "MusicRoom", "z#185.wav" },
		{ "MusicRoomLobby", "z#185.wav" },
		{ "1stClassRestaurant", "z#183.wav" },
	};

	if (msg->_value != -1) {
		int idx;
		for (idx = 0; idx < 8; ++idx) {
			if (compareRoomNameTo(ROOM_WAVES[idx][0])) {
				playSound(ROOM_WAVES[idx][1]);

			}
		}
		if (idx == 8)
			playSound("z#146.wav");

		sleep(2000);
	}

	playClip(getRandomNumber(1) ? "Whizz On Left" : "Whizz On Right",
		MOVIE_NOTIFY_OBJECT | MOVIE_GAMESTATE);
	movieEvent();
	_npcFlags |= NPCFLAG_400000;

	return true;
}

bool CDoorbot::TrueTalkTriggerActionMsg(CTrueTalkTriggerActionMsg *msg) {
	switch (msg->_action) {
	case 3:
		playClip("Cloak On", MOVIE_NOTIFY_OBJECT);
		break;

	case 4:
		_npcFlags = (_npcFlags & ~NPCFLAG_2) | NPCFLAG_4000000;
		playClip("Whizz Off Left", MOVIE_NOTIFY_OBJECT | MOVIE_GAMESTATE);
		break;

	case 28: {
		_npcFlags &= ~(NPCFLAG_2 | NPCFLAG_4);
		CDismissBotMsg dismissMsg;
		dismissMsg.execute(this);
		break;
	}
	}

	return true;
}

bool CDoorbot::DoorbotNeededInHomeMsg(CDoorbotNeededInHomeMsg *msg) {
	moveToView();
	setPosition(Point(90, 42));
	_npcFlags = NPCFLAG_100000;

	stopMovie();
	playClip("Cloak Off", MOVIE_NOTIFY_OBJECT);

	_npcFlags |= NPCFLAG_8;
	return true;
}

bool CDoorbot::DoorbotNeededInElevatorMsg(CDoorbotNeededInElevatorMsg *msg) {
	moveToView("ServiceElevator.Node 1.N");
	setPosition(Point(100, 42));

	if (_npcFlags & NPCFLAG_8000000) {
		_field108 = 7;
		_npcFlags |= NPCFLAG_200000;
		loadFrame(797);
	} else {
		_npcFlags = 0;
		if (msg->_value)
			endTalking(this, true);
	}

	return true;
}

bool CDoorbot::LeaveViewMsg(CLeaveViewMsg *msg) {
	if (!(_npcFlags & NPCFLAG_8000000) && (_npcFlags & NPCFLAG_400000)) {
		performAction(true);
		_npcFlags &= ~NPCFLAG_4;
	}

	return true;
}

bool CDoorbot::TimerMsg(CTimerMsg *msg) {
	if (msg->_action == "NPCIdleAnim") {
		return CTrueTalkNPC::TimerMsg(msg);
	} else if (_npcFlags & NPCFLAG_8000000) {
		switch (msg->_actionVal) {
		case 0:
			startTalking(this, 221475);
			break;

		case 1:
			startTalking(this, 221476);
			break;

		case 2:
			startTalking(this, 221477);
			break;

		case 3:
			playClip("DoubleTake Start", 0);
			playClip("DoubleTake End", 0);
			playClip("DoubleTake Start", 0);
			playClip("DoubleTake End", MOVIE_NOTIFY_OBJECT);
			_field108 = 3;
			break;

		case 4:
			startTalking(this, 221483);
			lockInputHandler();
			_field114 = true;
			break;

		case 5:
			lockInputHandler();
			mouseLockE4();
			_field114 = true;
			startTalking(this, 221485);
			break;

		case 6:
			CMouseButtonDownMsg::generate();
			mouseSetPosition(Point(200, 430), 2500);
			_timerId = addTimer(7, 2500, 0);
			break;

		case 7:
			CMouseButtonDownMsg::generate();
			startTalking(this, 221486);
			mouseUnlockE4();
			unlockInputHandler();
			_field114 = false;
			disableMouse();
			break;

		default:
			break;
		}
	} else if (msg->_action == "SummonBellbot") {
		CRoomItem *room = getRoom();
		if (room) {
			CSummonBotMsg botMsg;
			botMsg._npcName = "Bellbot";
			botMsg.execute(room);
		}

		_npcFlags &= ~NPCFLAG_4000000;
	}

	return true;
}

bool CDoorbot::NPCPlayTalkingAnimationMsg(CNPCPlayTalkingAnimationMsg *msg) {
	static const char *const NAMES1[] = {
		"Mutter Aside", "Rub Chin", "Drunken Eye Roll", "Drunken Head Move",
		"Look down and mutter",  "Look side to side", "Gesture forward and around",
		"Arms behind back", "Look down", "Rolling around", "Hold face",
		"Touch chin", "Cross hands in front", nullptr
	};
	static const char *const NAMES2[] = {
		"SE Talking 1", "SE Talking 2", "SE Talking 3", "SE Talking 4"
	};
	static const char *const NAMES3[] = {
		"SE Ask For Help", nullptr
	};

	if (msg->_value2 != 2) {
		if (_npcFlags & NPCFLAG_200000) {
			if (_field108 == 8 || _field110) {
				msg->_names = NAMES2;
			} else if (_field108 == 9) {
				msg->_names = NAMES3;
				_field108 = 10;
			}
		} else if (_npcFlags & (NPCFLAG_100000 | NPCFLAG_400000)) {
			msg->_names = NAMES1;
		}
	}

	return true;
}

bool CDoorbot::NPCPlayIdleAnimationMsg(CNPCPlayIdleAnimationMsg *msg) {
	static const char *const NAMES[] = {
		"Hand swivel", "Prompt Push", "Eye Roll", "Say something", nullptr
	};

	if (!(_npcFlags & (NPCFLAG_100000 | NPCFLAG_200000))
			&& (_npcFlags & NPCFLAG_400000))
		msg->_names = NAMES;

	return true;
}

bool CDoorbot::PutBotBackInHisBoxMsg(CPutBotBackInHisBoxMsg *msg) {
	petMoveToHiddenRoom();
	_npcFlags &= ~(NPCFLAG_4 | NPCFLAG_100000 | NPCFLAG_200000 | NPCFLAG_8000000);
	if (msg->_value)
		performAction(true);

	return true;
}

bool CDoorbot::DismissBotMsg(CDismissBotMsg *msg) {
	if (_npcFlags & NPCFLAG_400000) {
		playClip(getRandomNumber(1) ? "Whizz Off Left" : "Whizz Off Right",
			MOVIE_STOP_PREVIOUS | MOVIE_NOTIFY_OBJECT | MOVIE_GAMESTATE);
		movieEvent();

		if (_npcFlags & NPCFLAG_4) {
			_npcFlags &= ~NPCFLAG_4;
			performAction(true);
		} else {
			performAction(false);
		}

		CActMsg actMsg("DoorbotDismissed");
		actMsg.execute("BotIdleSummons");
	}

	return true;
}

bool CDoorbot::MovieFrameMsg(CMovieFrameMsg *msg) {
	if (clipExistsByStart("Whizz Off Left", msg->_frameNumber)
			|| clipExistsByStart("Whizz On Left", msg->_frameNumber)) {
		setPosition(Point(20, 42));
	} else if (clipExistsByStart("Whizz Off Right", msg->_frameNumber)
			|| clipExistsByStart("Whizz On Right", msg->_frameNumber)) {
		setPosition(Point(620 - _bounds.width(), 42));
	}

	return true;
}

bool CDoorbot::TrueTalkNotifySpeechEndedMsg(CTrueTalkNotifySpeechEndedMsg *msg) {
	if (_npcFlags & NPCFLAG_8000000) {
		switch (msg->_dialogueId) {
		case 10552:
			playClip("SE Try Buttons", MOVIE_NOTIFY_OBJECT);
			_field108 = 9;
			break;

		case 10553:
			enableMouse();
			break;

		case 10557:
			playClip("SE Move To Right", MOVIE_NOTIFY_OBJECT);
			_field108 = 11;
			break;

		case 10559:
			stopAnimTimer(_timerId);
			_timerId = addTimer(0, 2500, 0);
			break;

		case 10560:
			petShow();
			petSetArea(PET_CONVERSATION);
			stopAnimTimer(_timerId);
			_timerId = addTimer(1, 1000, 0);
			break;

		case 10561:
			enableMouse();
			_field108 = 1;
			stopAnimTimer(_timerId);
			_timerId = addTimer(2, 10000, 0);
			break;

		case 10562:
			if (_field108 == 1) {
				stopAnimTimer(_timerId);
				_timerId = addTimer(2, getRandomNumber(5000), 0);
			}
			break;

		case 10563:
		case 10564:
			disableMouse();
			startTalking(this, 221480);
			break;

		case 10565:
			startTalking(this, 221481);
			break;

		case 10566:
			stopAnimTimer(_timerId);
			_timerId = 0;
			if (_field110 == 2) {
				playClip("Cloak On", MOVIE_NOTIFY_OBJECT);
				_field108 = 6;
			} else {
				_timerId = addTimer(3, 2000, 0);
			}
			break;

		case 10567: {
			CActMsg actMsg("BecomeGettable");
			actMsg.execute("Photograph");
			enableMouse();
			stopAnimTimer(_timerId);
			_timerId = addTimer(4, 5000, 0);
			break;
		}

		case 10568:
			mouseLockE4();
			mouseSetPosition(Point(600, 250), 2500);
			_timerId = addTimer(6, 2500, 0);
			break;

		case 10569:
			if (_field110 != 2) {
				stopAnimTimer(_timerId);
				_timerId = addTimer(5, 3000, 0);
			}
			break;

		case 10570:
			mouseSetPosition(Point(200, 430), 2500);
			_timerId = addTimer(7, 3000, 0);
			break;

		case 10571:
			playClip("Cloak On", MOVIE_NOTIFY_OBJECT);
			_field108 = 6;
			break;

		default:
			break;
		}
	}

	return true;
}

bool CDoorbot::TextInputMsg(CTextInputMsg *msg) {
	if (!(_npcFlags & NPCFLAG_8000000))
		return CTrueTalkNPC::TextInputMsg(msg);

	if (_field108 == 1) {
		stopAnimTimer(_timerId);
		_field108 = 2;
		_timerId = 0;

		if (msg->_input == "yes" || msg->_input == "yeah"
				|| msg->_input == "yea" || msg->_input == "yup"
				|| msg->_input == "yep" || msg->_input == "sure"
				|| msg->_input == "alright" || msg->_input == "all right"
				|| msg->_input == "ok") {
			startTalking(this, 221479);
		} else {
			startTalking(this, 221478);
		}
	}

	return true;
}

bool CDoorbot::EnterViewMsg(CEnterViewMsg *msg) {
	if ((_npcFlags & NPCFLAG_8000000) && _field108 == 7)
		playClip("SE Move And Turn", MOVIE_NOTIFY_OBJECT);

	return true;
}

bool CDoorbot::ActMsg(CActMsg *msg) {
	if (msg->_action == "DoorbotPlayerPressedTopButton") {
		disableMouse();
		startTalking(this, 221471);
	} else if (msg->_action == "DoorbotPlayerPressedMiddleButton") {
		startTalking(this, 221470);
	}
	else if (msg->_action == "DoorbotPlayerPressedBottomButton") {
		startTalking(this, 221469);
	} else if (msg->_action == "DoorbotReachedEmbLobby") {
		startTalking(this, 221472);
	} else if (msg->_action == "PlayerPicksUpPhoto") {
		_field110 = 1;
		if (!_field114 && _field108 == 4) {
			stopAnimTimer(_timerId);
			_timerId = 0;
			_field108 = 5;
			startTalking(this, 221484);
		}
	} else if (msg->_action == "PlayerPutsPhotoInPet") {
		_field110 = 2;
		if (!_field114 && _field108 == 5) {
			stopAnimTimer(_timerId);
			_timerId = 0;
			startTalking(this, 221486);
			disableMouse();
		}
	}

	return true;
}

} // End of namespace Titanic
