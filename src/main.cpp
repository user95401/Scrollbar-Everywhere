#include <Geode/Geode.hpp>
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

using namespace geode::prelude;

void addScrollBarToNode(CCNode* self, float offset, CCNode* addTo) {
	if (self->getUserObject("scrollbar"_spr)) return;
	CCNode* parent = self->getParent();
	if (!addTo) addTo = parent;

	if (typeinfo_cast<GJListLayer*>(addTo)) {
		offset += 20;
	}

	CCScrollLayerExt* scrollLayer = reinterpret_cast<CCScrollLayerExt*>(self);

	Scrollbar* scrollbar = Scrollbar::create(scrollLayer);
	scrollbar->setID("scrollbar"_spr);
	scrollbar->setAnchorPoint(scrollLayer->getAnchorPoint());
	scrollbar->setZOrder(100);
	addTo->addChild(scrollbar);

	if (typeinfo_cast<AnchorLayout*>(parent->getLayout())) {

		Anchor anchor = Anchor::Right;
		CCPoint anchorPoint = {0.5, 0.5};
		if (AnchorLayoutOptions* options = typeinfo_cast<AnchorLayoutOptions*>(self->getLayoutOptions())) {
			if (options->getAnchor() == Anchor::Bottom) {
				anchor = Anchor::BottomRight;
				anchorPoint = CCPoint{0.5, 0};
			}
			if (options->getAnchor() == Anchor::Top) {
				anchor = Anchor::TopRight;
				anchorPoint = CCPoint{0.5, 1};
			}
		}
		scrollbar->setAnchorPoint(anchorPoint);
		scrollbar->setLayoutOptions(AnchorLayoutOptions::create()->setAnchor(anchor)->setOffset({offset, 0}));
		parent->updateLayout();
	}
	else {
		scrollbar->ignoreAnchorPointForPosition(scrollLayer->isIgnoreAnchorPointForPosition());
		scrollbar->setPosition(
			scrollLayer->getPositionX() + offset + scrollLayer->getContentWidth(),
			scrollLayer->getPositionY()
		);
	}

	//very evil, but is one size fits all since it is always added last and likely wont affect touch prio handling badly, here be dragons - Alphalaneous
	if (auto delegate = typeinfo_cast<CCTouchDelegate*>(scrollbar)) {
		if (auto handler = CCTouchDispatcher::get()->findHandler(delegate)) {
			CCTouchDispatcher::get()->setPriority(-1000, delegate);
		}
	}
};

void addScrollbar(CCNode* scrollLayer, float offset = 6.f, CCNode* addTo = nullptr) {
	if (addTo){
		addScrollBarToNode(scrollLayer, offset, addTo);
	}
	else {
		queueInMainThread([self = Ref(scrollLayer), offset, addToRef = Ref(addTo)] {
			addScrollBarToNode(self, offset, addToRef);
		});
	}
}

class ScrollbarProMax : public geode::Scrollbar {
	public:
	CCScrollLayerExt* getTarget() {
		return m_target;
	}
};

class $nodeModify(MyScrollBar, geode::Scrollbar) {
	void modify() {
		ScrollbarProMax* scrollbar = reinterpret_cast<ScrollbarProMax*>(this);
		scrollbar->getTarget()->setUserObject("scrollbar"_spr, scrollbar);
	}
};

class $nodeModify(MyScrollLayer, geode::ScrollLayer) {
	void modify() {
		addScrollbar(this, 7.5f);
	}
};

class $nodeModify(MyTableView, TableView) {
	void modify() {
		queueInMainThread([self = Ref(this)] {
			CCNode* parent = nullptr;
			if (CCNode* parent1 = self->getParent()) {
				if (CCNode* parent2 = parent1->getParent()) {
					parent = parent2;
				}
			}
			if (parent) {
				addScrollbar(self, 2.f, parent);
			}
		});
	}
};