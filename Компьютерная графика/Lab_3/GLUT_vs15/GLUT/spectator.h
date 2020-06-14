#pragma once

// Ќастройка наблюдател€
struct spectator_object {
	position LookAt, 
		LookFrom;
	
	type ratio;


	spectator_object() {
		this->LookFrom.x = 0;
		this->LookFrom.y = 0;
		this->LookFrom.z = 15;

		this->LookAt.x = 0;
		this->LookAt.y = 0;
		this->LookAt.z = 0;

		this->ratio = 1. * width / height;
	}
	
	// ”становка настроек наблюдател€
	void spectator_pos(type posX, type posY, type posZ,
		type viewX, type viewY, type viewZ)
	{
		//установить позицию камеры
		position pos = position(posX, posY, posZ);
		position view = position(viewX, viewY, viewZ);

		this->LookFrom = pos;
		this->LookAt = view;
	}

	// ƒвижение наблюдател€ вперед-назад
	void spectator_move(float speed)	{
		position vector_moving = this->LookAt - this->LookFrom;

		//ѕередвинуть камеру
		this->LookFrom.x += this->LookAt.x * speed;	//изменить положение 
		this->LookFrom.z += this->LookAt.z * speed;	//камеры

		this->LookAt.x += this->LookAt.x * speed;	//изменить направление 
		this->LookAt.z += this->LookAt.z * speed;	//взгл€да камеры
	}


}spect;