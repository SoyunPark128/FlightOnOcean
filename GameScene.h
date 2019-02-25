#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

#define TAG_SPRITE_PLAYER 1000
#define TAG_LABEL_SCORE 1001
#define TAG_LABEL_HIGHSCORE 1002

#define PADDING_SCREEN	10

class GameScene : public Layer
{
public:

	static Scene* createScene();

	virtual bool init();
	CREATE_FUNC(GameScene);

	void initBG();

	Size winSize;
	Point posStartTouch, posStartPlayer;

	Vector<Sprite*> items, enemies, missiles;

	bool isGetItem;
	int nScore, nScoreHigh;

	void initData();
	void initPlayer();
	
	void initScore();
	void addScore(int add);

	void setMissile(float delta);
	void resetMissile(Ref *sender);
	
	void setItem(float delta);
	void resetItem(Ref *sender);
	void resetGetItem(float delta);

	void setEnemy(float delta);
	void resetEnemy(Ref *sender);
	
	void resetBoom(Ref *sender);
	
	void update(float delta);
	

	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unsued_event);
};

#endif