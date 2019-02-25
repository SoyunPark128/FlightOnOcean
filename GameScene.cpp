#include "GameScene.h"

Scene* GameScene::createScene()
{
	auto scene = Scene::create();

	auto layer = GameScene::create();
	scene->addChild(layer);

	return scene;
}

bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 1);

	initBG();
	initData();
	initPlayer();
	initScore();

	this->scheduleUpdate();
	this->schedule(schedule_selector(GameScene::setMissile), 0.1);
	this->schedule(schedule_selector(GameScene::setItem), 5.0 + rand() % 4);
	this->schedule(schedule_selector(GameScene::setEnemy), 3.0 + rand() % 4);
	return true;
	
}

void GameScene::initBG()
{
	//auto bgLayer = LayerColor::create(Color4B(55,123,111,121));
	//this->addChild(bgLayer);
	auto bgLayer = Layer::create();
	this->addChild(bgLayer);

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 11; j++) {
			auto spr = Sprite::create("game/tile.png");
			spr->setAnchorPoint(Point::ZERO);
			spr->setPosition(Point(i * 33, j * 49));
			bgLayer->addChild(spr);
		}
	}
	auto action_0 = Sequence::create(MoveBy::create(0.5, Point(0, -49)),
		Place::create(Point::ZERO),
		NULL);

	auto action_1 = RepeatForever::create(action_0);
	bgLayer->runAction(action_1);
}

void GameScene::initData()
{
	winSize = Director::getInstance()->getWinSize();
	items.clear();
	enemies.clear();
	missiles.clear();
	isGetItem = false;
	nScore = 0;

	nScoreHigh = UserDefault::getInstance()->getIntegerForKey("HIGH_SCORE", 0);
}

void GameScene::initPlayer()
{
	auto spr = Sprite::create("game/player.png");
	spr->setPosition(Point(winSize.width / 2, winSize.height / 2));
	spr->setTag(TAG_SPRITE_PLAYER);
	this->addChild(spr, 1);
}

void GameScene::initScore()
{
	auto labelScore = Label::createWithSystemFont("", "", 14);
	labelScore->setAnchorPoint(Point(0, 1));
	labelScore->setPosition(Point(10, winSize.height - 10));
	labelScore->setColor(Color3B::BLACK);
	labelScore->setTag(TAG_LABEL_SCORE);
	this->addChild(labelScore, 100);

	auto labelHigh = Label::createWithSystemFont("", "", 14);
	labelHigh->setAnchorPoint(Point(1, 1));
	labelHigh->setPosition(Point(winSize.width - 10, winSize.height - 10));
	labelHigh->setColor(Color3B::MAGENTA);
	labelHigh->setTag(TAG_LABEL_HIGHSCORE);
	this->addChild(labelHigh, 100);

	addScore(0);
}

void GameScene::addScore(int add)
{
	nScore += add;

	if (nScore > nScoreHigh)
	{
		nScoreHigh = nScore;
		UserDefault::getInstance()->setIntegerForKey("HIGH_SCORE", nScoreHigh);
		UserDefault::getInstance()->flush();
	}

	auto labelScore = (Label*)this->getChildByTag(TAG_LABEL_SCORE);
	labelScore->setString(StringUtils::format("SCORE : %d", nScore));

	auto labelHigh = (Label*)this->getChildByTag(TAG_LABEL_HIGHSCORE);
	labelHigh->setString(StringUtils::format("HIGHSCORE : %d", nScoreHigh));

	
}

void GameScene::setMissile(float delta)
{
	auto sprPlayer = (Sprite*)this->getChildByTag(TAG_SPRITE_PLAYER);

	Sprite *sprMissile;

	if (isGetItem)
	{
		sprMissile = Sprite::create("game/missile_pow.png");
		sprMissile->setTag(5);
	}
	else
	{
		sprMissile = Sprite::create("game/missile.png");
		sprMissile->setTag(1);
	}

	sprMissile->setPosition(sprPlayer->getPosition() + Point(01, 20));
	this->addChild(sprMissile);

	missiles.pushBack(sprMissile);

	auto action = Sequence::create(MoveBy::create(1.0, Point(0, winSize.height)),
		CallFuncN::create(CC_CALLBACK_1(GameScene::resetMissile, this)),
		NULL);
	sprMissile->runAction(action);
}

void GameScene::resetMissile(Ref *sender)
{
	auto sprMissile = (Sprite*)sender;
	missiles.eraseObject(sprMissile);
	this->removeChild(sprMissile);
}

bool GameScene::onTouchBegan(Touch *touch, Event *unused_event)
{
	posStartTouch = touch->getLocation();

	auto sprPlayer = (Sprite*)this->getChildByTag(TAG_SPRITE_PLAYER);
	posStartPlayer = sprPlayer->getPosition();

	return true;
}

void GameScene::onTouchMoved(Touch *touch, Event *unsued_event)
{
	Point location = touch->getLocation();
	
	Point posChange = location - posStartTouch;

	auto sprPlayer = (Sprite*)this->getChildByTag(TAG_SPRITE_PLAYER);
	sprPlayer->setPosition(posStartPlayer + posChange);
}

void GameScene::setItem(float delta)
{
	int x = PADDING_SCREEN + rand() % ((int)winSize.width - PADDING_SCREEN * 2);
	auto sprItem = Sprite::create("game/item.png");
	sprItem->setPosition(Point(x, winSize.height));
	this->addChild(sprItem);

	items.pushBack(sprItem);

	auto action = Sequence::create(
		MoveBy::create(3.0, Point(0, -winSize.height)),
		CallFuncN::create(CC_CALLBACK_1(GameScene::resetItem, this)),
		NULL);
	sprItem->runAction(action);

}

void GameScene::resetItem(Ref *sender)
{
	auto sprItem = (Sprite*)sender;
	
	items.eraseObject(sprItem);

	this->removeChild(sprItem);
}

void GameScene::update(float delta)
{
	auto sprPlayer = (Sprite*)this->getChildByTag(TAG_SPRITE_PLAYER);
	Rect rectPlayer = sprPlayer->getBoundingBox();

	//CHECK THE COLISION OF PLAYER AND ITEMS

	auto removeItem = Sprite::create();

	for (Sprite* sprItem : items)
	{
		Rect rectItem = sprItem->getBoundingBox();
		if (rectPlayer.intersectsRect(rectItem))
		{
			auto particleGetItemEffect = ParticleFlower::create();
			particleGetItemEffect->setPosition(sprItem->getPosition());
			particleGetItemEffect->setSpeed(2 * (particleGetItemEffect->getSpeed()));
			particleGetItemEffect->setDuration(0.3);
			particleGetItemEffect->setColor(Color3B(0, 255, 122));
			this->addChild(particleGetItemEffect);

			removeItem = sprItem;
			resetItem(removeItem);

			isGetItem = true;
			this->scheduleOnce(schedule_selector(GameScene::resetGetItem), 5.0);
			break;
		}
	}
	/*	if (items.contains(removeItem))
		{
			resetItem(removeItem);
	}*/


	// CHECK THE COLISION OF MISSILE AND ENEMY

	auto removeMissile = Sprite::create();
	auto removeEnemy = Sprite::create();

	for (Sprite* sprMissile : missiles)
	{
		Rect rectMissile = sprMissile->getBoundingBox();

		for (Sprite* sprEnemy : enemies)
		{
			Rect rectEnemy = Rect(sprEnemy->getPositionX() - 10, sprEnemy->getPositionY() - 10, 20, 20);
			//Rect rectEnemy = sprEnemy->getBoundingBox();
			if (rectMissile.intersectsRect(rectEnemy))
			{
				int attack = sprMissile->getTag();
				int hp = sprEnemy->getTag();

				removeMissile = sprMissile;
				if (hp - attack > 0)
				{
					sprEnemy->setTag(hp - attack);
					addScore(1);
				}
				else
				{
					removeEnemy = sprEnemy;
					addScore(100);
				}
			}
		}
	}
	if (missiles.contains(removeMissile))
	{
		resetMissile(removeMissile);
	}

	if (enemies.contains(removeEnemy))
	{
		auto particleEnemyExplosionEffect = ParticleSystemQuad::create("game/explosion.plist");
		particleEnemyExplosionEffect->setPosition(removeEnemy->getPosition());
		//particleEnemyExplosionEffect->setSpeed(2 * (particleEnemyExplosionEffect->getSpeed()));
		//particleEnemyExplosionEffect->setDuration(0.3);
		particleEnemyExplosionEffect->setScale(0.5);
		this->addChild(particleEnemyExplosionEffect);

		auto action = Sequence::create(DelayTime::create(1.0),
			CallFuncN::create(CC_CALLBACK_1(GameScene::resetBoom, this)),
			NULL);
		particleEnemyExplosionEffect->runAction(action);
		
		resetEnemy(removeEnemy);
	}
}

void GameScene::resetGetItem(float delta)
{
	isGetItem = false;
}

void GameScene::setEnemy(float delta)
{
	int x = PADDING_SCREEN + rand() % ((int)winSize.width - PADDING_SCREEN * 2);

	Sprite * sprEnemy;

	if ((rand() % 100) < 30)
	{
		sprEnemy = Sprite::create("game/boss.png");
		sprEnemy->setTag(100);
	}
	else
	{
		sprEnemy = Sprite::create("game/enemy.png");
		sprEnemy->setTag(10);
	}


	sprEnemy->setPosition(Point(x, winSize.height));
	this->addChild(sprEnemy);

	enemies.pushBack(sprEnemy);

	auto action = Sequence::create(
		MoveBy::create(10.0, Point(0, -winSize.height)),
		CallFuncN::create(CC_CALLBACK_1(GameScene::resetEnemy, this)),
		NULL);
	sprEnemy->runAction(action);
}

void GameScene::resetEnemy(Ref *sender)
{
	auto sprEnemy = (Sprite*)sender;
	enemies.eraseObject(sprEnemy);
	this->removeChild(sprEnemy);
}

void GameScene::resetBoom(Ref *sender)
{
	auto particle = (ParticleSystemQuad*)sender;
	this->removeChild(particle);
}