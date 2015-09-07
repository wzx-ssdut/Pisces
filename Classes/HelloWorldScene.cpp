#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene() {
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init() {
    if (!Layer::init()) {
        return false;
    }

    auto sp1 = Sprite::create("HelloWorld.png");
    sp1->setNormalizedPosition(Vec2(0.25f, 0.25f));
    sp1->setScale(0.5f);
    sp1->setSkewX(45.0f);
    this->addChild(sp1);

    auto sp2 = Sprite::create("HelloWorld.png");
    sp2->setNormalizedPosition(Vec2(0.75f, 0.25f));
    sp2->setScale(0.5f);
    sp2->setRotationSkewX(45.0f);
    this->addChild(sp2);

    /*
    auto sp3 = Sprite::create("HelloWorld.png");
    sp3->setNormalizedPosition(Vec2(0.75f, 0.75f));
    sp3->setScale(0.5f);
    sp3->setRotationSkewY(45.0f);
    this->addChild(sp3);
    */

    return true;
}
