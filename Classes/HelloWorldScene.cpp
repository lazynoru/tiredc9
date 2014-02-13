#include "HelloWorldScene.h"
#include "Box2D.h"

#define FLOOR_HEIGHT    100
#define PTM_RATIO       32

USING_NS_CC;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRet = false;
    _targets = new CCArray;
    _projectiles = new CCArray;
    
    do {
    //////////////////////////////
    // 1. super init first

    CC_BREAK_IF(! CCLayer::init());
    
    b2Vec2 gravity(0.0f, -10.0f);
    _myWorld = new b2World(gravity);
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        
    // bg
    CCSprite *bg = CCSprite::create("bg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    bg->setAnchorPoint(CCPointZero);
//    bg->setPosition(CCPointMake(0.0, 0.0));
    this->addChild(bg, -1);
        
    // sea
        b2BodyDef seaBodyDef;
        seaBodyDef.position.Set(0, 0);
        seaBodyDef.type = b2_kinematicBody;
        b2Body *seaBody = _myWorld->CreateBody(&seaBodyDef);

        b2EdgeShape seaEdge;
        seaEdge.Set(b2Vec2(0, FLOOR_HEIGHT/PTM_RATIO), b2Vec2(winSize.width/PTM_RATIO, FLOOR_HEIGHT/PTM_RATIO));
        b2FixtureDef seaShapeDef;
        seaShapeDef.shape = &seaEdge;
        
        seaBody->CreateFixture(&seaShapeDef);
        
    // ship
    CCSprite *ship = CCSprite::create("paddle-hd.png", CCRectMake(0, 0, 240, 90));
    ship->setPosition(ccp(winSize.width/2, winSize.height/2));
    this->addChild(ship);
        
        b2Vec2 vertices[4];
        vertices[0].Set(0.0f, 0.0f);
        vertices[1].Set(1.0f, 0.0f);
        vertices[2].Set(1.0f, 1.0f);
        vertices[3].Set(0.0f, 1.0f);
        int count = 4;
        
        b2PolygonShape polygon;
        polygon.Set(vertices, count);
        
        
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(winSize.width/2/32, winSize.height/2/32);
    bodyDef.userData = ship;
    b2Body *body = _myWorld->CreateBody(&bodyDef);
        
    b2PolygonShape groundBox;
    groundBox.SetAsBox(1.0f, 1.0f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &groundBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.8f;
    body->CreateFixture(&fixtureDef);
        
    //close menu
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create("CloseNormal.png","CloseSelected.png",this,menu_selector(HelloWorld::menuCloseCallback));

    CC_BREAK_IF(! pCloseItem);

    pCloseItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width-20, 20));

    CCMenu *pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    CC_BREAK_IF(!pMenu);

    this->addChild(pMenu, 1);
        
    CCLayerColor::initWithColor(ccc4(0, 0, 0, 0));
//        if (CCLayerColor::initWithColor(ccc4(255, 255, 255, 255)))
//        {
//            CCSize winSize = CCDirector::sharedDirector()->getWinSize();
//            CCSprite *player = CCSprite::create("Player.png", CCRectMake(0,0,27,40));
//            player->setPosition(ccp(player->getContentSize().width/2, winSize.height/2));
//            this->addChild(player);
//        }
    
    bRet = true;
        
        this->schedule(schedule_selector(HelloWorld::gameLogic));
//        this->setTouchEnabled(true);
//        _myWorld.Step(1.0f/60.0f, 6, 2);
        
//        this->schedule(schedule_selector(HelloWorld::update));
    
    } while (0);

    return bRet;
}

// cpp with cocos2d-x
void HelloWorld::update(float dt)
{
    CCArray *projectilesToDelete = new CCArray;
    CCArray* targetsToDelete =new CCArray;
    CCObject* it = NULL;
    CCObject* jt = NULL;
    
    CCARRAY_FOREACH(_projectiles, it)
    {
        CCSprite *projectile = dynamic_cast<CCSprite*>(it);
        CCRect projectileRect = CCRectMake(
                                           projectile->getPosition().x - (projectile->getContentSize().width/2),
                                           projectile->getPosition().y - (projectile->getContentSize().height/2),
                                           projectile->getContentSize().width,
                                           projectile->getContentSize().height);
        
        
        CCARRAY_FOREACH(_targets, jt)
        {
            CCSprite *target = dynamic_cast<CCSprite*>(jt);
            CCRect targetRect = CCRectMake(
                                           target->getPosition().x - (target->getContentSize().width/2),
                                           target->getPosition().y - (target->getContentSize().height/2),
                                           target->getContentSize().width,
                                           target->getContentSize().height);
            
            if (projectileRect.intersectsRect(targetRect))
            {
                targetsToDelete->addObject(target);
                projectilesToDelete->addObject(projectile);
            }
        }
    }
    
    CCARRAY_FOREACH(targetsToDelete, jt)
    {
        CCSprite *target = dynamic_cast<CCSprite*>(jt);
        _targets->removeObject(target);
        this->removeChild(target, true);
    }
    
    CCARRAY_FOREACH(projectilesToDelete, it)
    {
        CCSprite* projectile = dynamic_cast<CCSprite*>(it);
        _projectiles->removeObject(projectile);
        this->removeChild(projectile, true);
    }
    
    
    projectilesToDelete->release();
    targetsToDelete->release();
}

HelloWorld::~HelloWorld()
{
    if (_targets) {
        _targets->release();
        _targets = NULL;
    }
    
    if (_projectiles) {
        _projectiles->release();
        _projectiles = NULL;
    }
}

HelloWorld::HelloWorld() : _targets(NULL), _projectiles(NULL), _myWorld(NULL)
{
}

void HelloWorld::gameLogic(float dt)
{
//    this->addTarget();
    _myWorld->Step(dt, 10, 10);
    for (b2Body *body = _myWorld->GetBodyList(); body; body = body->GetNext()) {
        if (body -> GetUserData() != NULL) {
            CCSprite *ballData = (CCSprite *)body->GetUserData();
            ballData->setPosition(ccp(body->GetPosition().x*32.0, body->GetPosition().y*32.0));
        }
    }
}

void HelloWorld::ccTouchesEnded(cocos2d::CCSet *touches, cocos2d::CCEvent *event)
{
    CCTouch *touch = (CCTouch *)(touches->anyObject());
    CCPoint location = touch->getLocationInView();
    location = CCDirector::sharedDirector()->convertToGL(location);
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCSprite *projectile = CCSprite::create("Projectile.png", CCRectMake(0, 0, 20, 20));
    projectile->setPosition(ccp(20, winSize.height/2));
    
    int offX = location.x - projectile->getPosition().x;
    int offY = location.y - projectile->getPosition().y;

    if (offX <= 0) return;
    
    this->addChild(projectile);
    
    int realX = winSize.width + (projectile->getContentSize().width/2);
    float ratio = (float)offY / (float)offX;
    int realY = (realX * ratio) +projectile->getPosition().y;
    CCPoint realDest = ccp(realX, realY);
    
    int offRealX = realX - projectile->getPosition().x;
    int offRealY = realY - projectile->getPosition().y;
    float length = sqrtf((offRealX * offRealX) + (offRealY * offRealY));
    float velocity = 480/1;
    float realMoveDuration = length/velocity;
    
    projectile->runAction( CCSequence::create(
                                              CCMoveTo::create(realMoveDuration, realDest),
                                              CCCallFuncN::create(this,
                                                                  
                                                                  callfuncN_selector(HelloWorld::spriteMoveFinished)), 
                                              NULL) );
    
    projectile->setTag(2);
    _projectiles->addObject(projectile);
    
}

void HelloWorld::spriteMoveFinished(CCNode *sender)
{
    CCSprite *sprite = (CCSprite*)sender;
    this->removeChild(sprite, true);
    
    if (sprite->getTag() == 1) {
        _targets->removeObject(sprite);
    } else if (sprite->getTag() == 2)
    {
        _targets->removeObject(sprite);
    }
    
}

void HelloWorld::addTarget()
{
    CCSprite *target = CCSprite::create("Target.png", CCRectMake(0, 0, 27, 40));
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    int minY = target->getContentSize().height/2;
    int maxY = winSize.height - target->getContentSize().height/2;
    int rangeY = maxY - minY;
    int actualY = (rand()%rangeY) + minY;
    
    target->setPosition(ccp(winSize.width + target->getContentSize().width/2, actualY));
    this->addChild(target);
    
    int minDuration = (int)2.0;
    int maxDuration = (int)4.0;
    
    int rangeDuration = maxDuration - minDuration;
    int actualDuration = (rand()%rangeDuration) + minDuration;
    
    CCFiniteTimeAction *actionMove = CCMoveTo::create((float)actualDuration, ccp(0 - target->getContentSize().width/2, actualY));
    CCFiniteTimeAction *actionMoveDone = CCCallFuncN::create(this, callfuncN_selector(HelloWorld::spriteMoveFinished));
    target->runAction(CCSequence::create(actionMove, actionMoveDone, NULL));
    
    target->setTag(1);
    _targets->addObject(target);
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}
