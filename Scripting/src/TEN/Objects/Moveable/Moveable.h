#pragma once

#include "Objects/NamedBase.h"

namespace sol {
	class state;
	template <typename T> struct as_table_t;
}
class Position;
class Rotation;
struct ITEM_INFO;
enum GAME_OBJECT_ID : short;

using aiBitsArray = std::array<int, 6>;
using aiBitsType = sol::as_table_t<aiBitsArray>;

class Moveable : public NamedBase<Moveable, short>
{
public:
	using IdentifierType = short;

	Moveable(short num, bool init = true);
	~Moveable();
	Moveable& operator=(Moveable const& other) = delete;
	Moveable(Moveable const& other) = delete;
	Moveable(Moveable && other) noexcept;

	static void Register(sol::table & parent);

	GAME_OBJECT_ID GetObjectID() const;
	void SetObjectID(GAME_OBJECT_ID id);

	std::string GetName() const;
	bool SetName(std::string const &);

	bool GetValid() const;
	void Invalidate();

	void Destroy();

	Position GetPos() const;
	void SetPos(Position const& pos);

	Rotation GetRot() const;
	void SetRot(Rotation const& rot);

	int GetAnimNumber() const;
	void SetAnimNumber(int animNumber);

	int GetFrameNumber() const;
	void SetFrameNumber(int frameNumber);

	short GetHP() const;
	void SetHP(short hp);

	short GetOCB() const;
	void SetOCB(short ocb);

	aiBitsType GetAIBits() const;
	void SetAIBits(aiBitsType const & bits);

	bool GetHitStatus() const;

	bool GetActive() const;
	void SetActive(bool active);

	short GetRoom() const;
	void SetRoom(short Room);

	void EnableItem();
	void DisableItem();
	void MakeInvisible();

	void SetOnHit(sol::protected_function);
	void SetOnKill(sol::protected_function);

	short GetStatus() const;

	void Init();

	void CallOnHit();
	void CallOnKill();

private:
	ITEM_INFO* m_item;
	short m_num;
	bool m_initialised;
	sol::protected_function	m_onHit{};
	sol::protected_function	m_onKill{};
};