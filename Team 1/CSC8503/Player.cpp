#include "Player.h"

#include "TutorialGame.h"
#include "GameWorld.h"

#include "Controller.h"

#include "RenderObject.h"
#include "Gamepad.h"
#include "KeyboardController.h"

Player::Player(GameWorld* w, TutorialGame* g, int id, int t, Camera* cam, bool hasController) : GameObject("Player") {
	SoundsManager::AddSound((Assets::SOUNDSDIR + "walk.wav").c_str());
	walkAudio = SoundsManager::GetSound((Assets::SOUNDSDIR + "walk.wav").c_str());
	walkAud = new SoundSource();
	SoundsManager::AddSound((Assets::SOUNDSDIR + "jump.wav").c_str());
	jumpAudio = SoundsManager::GetSound((Assets::SOUNDSDIR + "jump.wav").c_str());
	jumpAud = new SoundSource();
	SoundsManager::AddSound((Assets::SOUNDSDIR + "paint.wav").c_str());
	paintAudio = SoundsManager::GetSound((Assets::SOUNDSDIR + "paint.wav").c_str());
	paintAud = new SoundSource();
	SoundsManager::AddSound((Assets::SOUNDSDIR + "cleaning.wav").c_str());
	cleanAudio = SoundsManager::GetSound((Assets::SOUNDSDIR + "cleaning.wav").c_str());
	cleanAud = new SoundSource();

	SoundSystem::GetSoundSystem()->AddSoundEmitter(walkAud);
	SoundSystem::GetSoundSystem()->AddSoundEmitter(jumpAud);
	SoundSystem::GetSoundSystem()->AddSoundEmitter(paintAud);
	SoundSystem::GetSoundSystem()->AddSoundEmitter(cleanAud);

	isBlind = false;

	world = w;
	camera = cam;
	yaw = camera->GetYaw();
	pitch = camera->GetPitch();
	currentItem = Speed;
	team = t;
	game = g;
	ID = id;
	cameraFwd = Vector3();
	controller = new KeyboardController(0);
	
	//canSlide = true;
	//slideTime = 0;
	paintTimer = MAX_PAINT_TIME;

	ID = id;
	cameraFwd = Vector3();

	if (hasController) {
		controller = new Gamepad(id);
	}
	else {
		controller = new KeyboardController(1);
	}
	
	effect = 0;
	buffer[0] = 0;

	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(int), &buffer[0], GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	ssboPtr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

Player::Player(GameWorld* w, TutorialGame* g, int t, int id, int hasController) : GameObject("Player") {
	SoundsManager::AddSound((Assets::SOUNDSDIR + "walk.wav").c_str());
	walkAudio = SoundsManager::GetSound((Assets::SOUNDSDIR + "walk.wav").c_str());
	walkAud = new SoundSource();
	SoundsManager::AddSound((Assets::SOUNDSDIR + "jump.wav").c_str());
	jumpAudio = SoundsManager::GetSound((Assets::SOUNDSDIR + "jump.wav").c_str());
	jumpAud = new SoundSource();
	SoundsManager::AddSound((Assets::SOUNDSDIR + "paint.wav").c_str());
	paintAudio = SoundsManager::GetSound((Assets::SOUNDSDIR + "paint.wav").c_str());
	paintAud = new SoundSource();
	SoundsManager::AddSound((Assets::SOUNDSDIR + "cleaning.wav").c_str());
	cleanAudio = SoundsManager::GetSound((Assets::SOUNDSDIR + "cleaning.wav").c_str());
	cleanAud = new SoundSource();

	SoundSystem::GetSoundSystem()->AddSoundEmitter(walkAud);
	SoundSystem::GetSoundSystem()->AddSoundEmitter(jumpAud);
	SoundSystem::GetSoundSystem()->AddSoundEmitter(paintAud);
	SoundSystem::GetSoundSystem()->AddSoundEmitter(cleanAud);

	isBlind = false;

	world = w;
	camera = world->GetCamera(id);
	yaw = camera->GetYaw();
	pitch = camera->GetPitch();
	currentItem = Speed;
	team = t;
	game = g;

	//canSlide = true;
	slideTime = 0;
	paintTimer = MAX_PAINT_TIME;

	ID = id;
	cameraFwd = Vector3();
	if (hasController)
		controller = new Gamepad(id - 1);
	else
		controller = new KeyboardController(1);

	effect = 0;
	buffer[0] = 0;
	
	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(int), &buffer[0], GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	ssboPtr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

Player::~Player() {
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	walkAud->SoundSource::DetachSource();
	jumpAud->SoundSource::DetachSource();
	paintAud->SoundSource::DetachSource();
	cleanAud->SoundSource::DetachSource();
	delete controller;
}

void Player::Update(float dt) {
	PlayerCamera(dt);
	PlayerMovement(dt);
	PlayerActions(dt);

	if (GetTransform().GetPosition().y <= -3) {
		GetTransform().SetPosition({ 0, 3, 0 });
	}
}

void Player::PlayerMovement(float dt) {
	Ray isGround(GetTransform().GetPosition(), Vector3(0, -1, 0));
	RayCollision coll;
	if (world->Raycast(isGround, coll, true, this)) {
		onGround = (coll.rayDistance < 1.5f);
	}	

	keyPress[0] = (controller->GetInputDown(c_UP));
	keyPress[1] = (controller->GetInputDown(c_DOWN));
	keyPress[2] = (controller->GetInputDown(c_LEFT));
	keyPress[3] = (controller->GetInputDown(c_RIGHT));
	keyPress[4] = (controller->GetInputDown(c_A) && onGround);
	keyPress[7] = (controller->GetInputDown(c_B) && canSlide && onGround);
	
	Vector3 direction = (Vector3(0, 0, -1) * keyPress[0])
						+ (Vector3(0, 0, 1) * keyPress[1])
						+ (Vector3(-1, 0, 0) * keyPress[2]) 
						+ (Vector3(1, 0, 0) * keyPress[3])
						+ (Vector3(0, 5, 0) * keyPress[4]);


	// player can slide every 5 seconds (0.75s slide, 2.5x speed)
	if (keyPress[7] && fiveSecondsHasPassed) { 
		canSlide = false;
		slideTime = MAX_SLIDE_TIME;
	}

	if (canSlide == false) {
		fiveSeconds -= dt;
	}

	if (!canSlide && !keyPress[7]) {
		fiveSecondsHasPassed = false;
		if (slideTime > 0) {
			slideTime -= dt;
			direction *= 2.5f;
			camera->SetPosition(camera->GetPosition() - Vector3(0, CAMERA_Y_OFFSET, 0));
		}
		else if (slideTime <= 0 && fiveSeconds <= 0.0f) {
			canSlide = true;
			fiveSeconds = 5.0f;
			fiveSecondsHasPassed = true;
		}
	}

	bool keyPressed = direction != Vector3() ? true : false;

	if (onGround) {
		auto startTime = std::chrono::steady_clock::now();
		DetectPaint();
		auto endTime = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed = endTime - startTime;
		game->SetDetectPaintTime(elapsed.count());

		if (direction != Vector3()) {
			walkAud->SetSound(walkAudio);
			walkAud->SetRadius(1.0f);
		}
	}
	else {
		effect = 0;
	}

	ApplyMovement(dt, direction, effect);
}

void Player::PlayerCamera(float dt) {
	yaw = controller->GetYaw(yaw);
	pitch = controller->GetPitch(pitch);

	pitch = min(pitch, 90.0f);
	pitch = max(pitch, -90.0f);

	if (yaw < 0) 
		yaw += 360.0f;
	if (yaw > 360.0f) 
		yaw -= 360.0f;
	
	float radius = -1.0f;
	Matrix4 temp = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Quaternion q(temp);
	GetTransform().SetOrientation(q);
	camera->SetPitch(pitch);
	camera->SetYaw(yaw);

	float x = GetTransform().GetPosition().x + radius * sin(yaw * 3.14f / 180);
	float z = GetTransform().GetPosition().z + radius * cos(yaw * 3.14f / 180);
	float y = GetTransform().GetPosition().y - radius * sin(pitch * 3.14f / 180);

	Vector3 camPos = Vector3(x, y + CAMERA_Y_OFFSET, z);
	camera->SetPosition(camPos);

	Vector4 tmp = camera->BuildViewMatrix().GetRow(2);
	cameraFwd = -Vector3(tmp.x, tmp.y, tmp.z);
}

void Player::ApplyMovement(const float dt, const Vector3 direction, const int effect) {	
	switch (effect) {
	case 1:
		GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * direction * (speed * 3.0f));
		break;
	case 2:
		GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * direction * (speed * 0.5f));
		break;
	case 3:
		GetPhysicsObject()->SetElasticity(2.0f);
		GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * Vector3(direction.x, 30.0f, direction.z) * speed);
		break;
	case 4:
		GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * -direction * speed);
		break;
	case 5:
		// splat paint
		GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * direction * speed);
		GetPhysicsObject()->SetElasticity(0.8f);
		isBlind = true;
		blindTimer = 5.0f;
		break;
	default:
		GetPhysicsObject()->AddForce(GetTransform().GetOrientation() * direction * speed);
		GetPhysicsObject()->SetElasticity(0.8f);
		break;
	}

	blindTimer -= dt;
	if (blindTimer <= 0) {
		isBlind = false;
		blindTimer = 0;
	}
}

void Player::PlayerActions(float dt) {
	if (controller->GetInputPressed(c_RIGHTBUMPER))
		currentItem = ((currentItem + 1)  % (Last - 1)) != 0 ? ((currentItem + 1) % (Last - 1)) : Last - 1;;
	if (controller->GetInputPressed(c_LEFTBUMPER))
		currentItem = ((currentItem - 1) % (Last - 1)) != 0 ? ((currentItem - 1) % (Last - 1)) : Last -1;

	if (controller->GetTriggerDown(1) && paintTimer > 0) {
		Vector3 colour = Vector3();
		switch (currentItem) {
		case Speed:
			colour = Vector3(1, 0, 0);
			break;
		case Slow:
			colour = Vector3(0, 0, 1);
			break;
		case Bounce:
			colour = Vector3(0, 1, 0);
			break;
		case Confusion:
			colour = Vector3(1, 0, 1);
			break;
		case Splat:
			colour = Vector3(0, 1, 1);
			break;
		}
		auto startTime = std::chrono::steady_clock::now();
		game->ApplyPaint(colour, camera->GetPosition(), GetCameraFwd(), this);
		auto endTime = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed = endTime - startTime;
		game->SetApplyPaintTime(elapsed.count());
		keyPress[5] = true;
		paintTimer -= dt;
		releasedTimer = MAX_RELEASE_TIME;
	}
	else {
		keyPress[5] = false;
	}

	if (controller->GetTriggerDown(0) && paintTimer > 0) {
		auto startTime = std::chrono::steady_clock::now();
		game->ApplyPaint(Vector3(0, 0, 0), camera->GetPosition(), GetCameraFwd(), this);
		auto endTime = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed = endTime - startTime;
		game->SetApplyPaintTime(elapsed.count());
		cleanAud->SetSound(cleanAudio);
		keyPress[6] = true;
		paintTimer -= dt;
		releasedTimer = MAX_RELEASE_TIME;
	}
	else {
		keyPress[6] = false;
	}

	if (!controller->GetTriggerDown(1)) {
		if (releasedTimer <= 0) paintTimer += paintTimer < MAX_PAINT_TIME ? dt : 0;
		else releasedTimer -= dt;
	}
}

void Player::DetectPaint() {
	Ray ray = Ray(GetTransform().GetPosition(), Vector3(0, -1, 0));
	RayCollision closestCollision;

	if (world->Raycast(ray, closestCollision, true, this)) {
		
		GameObject* obj = (GameObject*)closestCollision.node;

		if (obj->GetRenderObject()->GetPaintMaskTexture() == nullptr) return;

		AssetManager::detectionShader->Bind();

		OGLMesh* mesh = (OGLMesh*)(obj)->GetRenderObject()->GetMesh();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh->attributeBuffers[VertexAttribute::Positions]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, mesh->attributeBuffers[VertexAttribute::TextureCoords]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mesh->indexBuffer);

		Vector3 localSpacePos = obj->GetTransform().GetMatrix().Inverse() * closestCollision.collidedAt;
		TextureBase* paintMask = obj->GetRenderObject()->GetPaintMaskTexture();

		glBindImageTexture(1, ((OGLTexture*)paintMask)->GetObjectID(), 0, GL_FALSE, NULL, GL_READ_ONLY, GL_RGBA32F);

		int localPosLocation = glGetUniformLocation(AssetManager::detectionShader->GetProgramID(), "localPos");
		glUniform3fv(localPosLocation, 1, (float*)&localSpacePos);

		int ssboLocation = glGetProgramResourceIndex(AssetManager::detectionShader->GetProgramID(), GL_SHADER_STORAGE_BLOCK, "effects");
		glShaderStorageBlockBinding(AssetManager::detectionShader->GetProgramID(), ssboLocation, 9);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, SSBO);

		AssetManager::detectionShader->Execute((mesh->GetIndexCount() / 3) / 64 + 1, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		effect = ((int*)ssboPtr)[0];

		AssetManager::detectionShader->Unbind();
	}
}