#include <Geode/Geode.hpp>

using namespace geode::prelude;

#define public_cast(value, member) [](auto* v) { \
	class FriendClass__; \
	using T = std::remove_pointer<decltype(v)>::type; \
	class FriendeeClass__: public T { \
	protected: \
		friend FriendClass__; \
	}; \
	class FriendClass__ { \
	public: \
		auto& get(FriendeeClass__* v) { return v->member; } \
	} c; \
	return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)

#include <Geode/modify/CCNode.hpp>
class $modify(ScrollbarsExt, CCNode) {

	void addChild(CCNode * pChild) {

		CCNode::addChild(pChild);

		Ref _child = pChild;
		Ref _this = this;

		queueInMainThread(
			[_this, _child] {
				if (!_this or !_child) return;

				if (Ref bar = typeinfo_cast<Scrollbar*>(_child.data())) {

					findFirstChildRecursive<Scrollbar>(
						_this->getParent() ? _this->getParent() : _this, [_this, bar](Scrollbar* asd) {
							if (!_this or !bar or !asd) return false;
							
							//fucking protected members.
							Ref main_tar = public_cast(bar.data(), m_target);
							Ref this_tar = public_cast(asd, m_target);

							if (asd != bar) if (main_tar == this_tar) {
								if (asd and string::contains(Ref(asd)->getID(), getMod()->getID())) {
									Ref(asd)->removeFromParentAndCleanup(0);
								}
							};

							return false;
						}
					);

				}

				if (Ref scroll = typeinfo_cast<CCScrollLayerExt*>(_child.data())) {

					auto offset = 6.f;
					if ("special offsets") {
						auto& q = offset;
						auto id = !_this ? "" : _this->getID();
						auto pID = !_this->getParent() ? "" : _this->getParent()->getID();
						auto pTYPE = std::string(!_this->getParent() ? pID : typeid(*_this->getParent()).name());
						q = (id == "ModList") ? (412.000 - 350.000) : q;
						q = (id == "list-view") ? (348.000 - 340.000) : q;
						q = (string::contains(pTYPE, "GJListLayer")) ? (385.000 - 356.000) : q;
					}

					if (!findFirstChildRecursive<Scrollbar>(
						_this->getParent() ? _this->getParent() : _this, [scroll](Scrollbar* asd) {
							//fucking protected members.
							Ref target = public_cast(asd, m_target);
							return target.data() == scroll.data();
						}
					)) {
						Ref scrollbar = Scrollbar::create(scroll);
						scrollbar->setPosition(
							scroll->getPositionX() + offset + scroll->getContentWidth(),
							scroll->getPositionY() + scroll->getContentHeight() / 2
						);
						scrollbar->setID("scrollbar"_spr);
						_this->insertAfter(scrollbar, _child);
						handleTouchPriorityWith(scrollbar, -1000, 1);
					};

				};//scroll

			}//queue lb
		);//queueInMainThread

	}//addChild

};