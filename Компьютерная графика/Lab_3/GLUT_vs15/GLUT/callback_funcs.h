#pragma once

void Mouse_move(int x, int y)
{
	if (System_.rotation_mode)
		return
		//spect.LookAt();;
		;
}

void Keys_(unsigned char key, int x, int y)
{
	if (key == ESC)
		glutLeaveMainLoop();
	/*
	//включение/выключение вращения камеры мышью
	if (key == 'q' || key == 'Q')
	{
		//rot = !rot;
		//ShowCursor(!rot);
	}

	//включение/выключение трассировки
	if (key == 't' || key == 'T')
	{
		//raytracer_mode = !raytracer_mode;
		//if (!raytracer_mode)
			//Reshape(width, height);
	}

	//включение/выключение режима включения/исключения фигур
	if (key == 'c' || key == 'C')
	{
		//переключить режим
		//scene1.add_del = !scene1.add_del;

		//если режим включен
		if (scene1.add_del)
		{
			//задать начальные установки
			scene1.sphere_mod = true;
			scene1.tetrahedron_mod = false;
			scene1.active_Sphere = 0;
			scene1.active_Tetrahedron = 0;
		}

		//иначе
		else
		{
			scene1.sphere_mod = false;
			scene1.tetrahedron_mod = false;
		}
	}

	//включить исключить фигуру
	if (key == 'e' || key == 'E')
	{
		//если включен режив включения/исключения фигур
		if (scene1.add_del)
		{
			if (scene1.sphere_mod)
				scene1.vector_Sphere[scene1.active_Sphere].display = !scene1.vector_Sphere[scene1.active_Sphere].display;

			if (scene1.tetrahedron_mod)
				scene1.vector_Tetrahedron[scene1.active_Tetrahedron].display = !scene1.vector_Tetrahedron[scene1.active_Tetrahedron].display;
		}
	}

	//движение камеры
	if (key == 'w' || key == 'W')
	{
		camera1.MoveCamera(kSpeed);
	}
	if (key == 's' || key == 'S')
	{
		camera1.MoveCamera(-kSpeed);
	}
	if (key == 'a' || key == 'A')
	{
		camera1.RotateAroundPoint(camera1.View, -kSpeed * 2.0f, 0.0f, 1.0f, 0.0f);
	}
	if (key == 'd' || key == 'D')
	{
		camera1.RotateAroundPoint(camera1.View, kSpeed * 2.0f, 0.0f, 1.0f, 0.0f);
	}

	//перемещение источника света
	if (key == '1')
	{
		scene1.vector_Light[0].position.y++;
		printf("%0.0f %0.0f %0.0f\n", scene1.vector_Light[0].position.x, scene1.vector_Light[0].position.y, scene1.vector_Light[0].position.z);
	}
	if (key == '2')
	{
		scene1.vector_Light[0].position.y--;
		printf("%0.0f %0.0f %0.0f\n", scene1.vector_Light[0].position.x, scene1.vector_Light[0].position.y, scene1.vector_Light[0].position.z);
	}
	if (key == '3')
	{
		scene1.vector_Light[0].position.x++;
		printf("%0.0f %0.0f %0.0f\n", scene1.vector_Light[0].position.x, scene1.vector_Light[0].position.y, scene1.vector_Light[0].position.z);
	}
	if (key == '4')
	{
		scene1.vector_Light[0].position.x--;
		printf("%0.0f %0.0f %0.0f\n", scene1.vector_Light[0].position.x, scene1.vector_Light[0].position.y, scene1.vector_Light[0].position.z);
	}
	if (key == '5')
	{
		scene1.vector_Light[0].position.z++;
		printf("%0.0f %0.0f %0.0f\n", scene1.vector_Light[0].position.x, scene1.vector_Light[0].position.y, scene1.vector_Light[0].position.z);
	}
	if (key == '6')
	{
		scene1.vector_Light[0].position.z--;
		printf("%0.0f %0.0f %0.0f\n", scene1.vector_Light[0].position.x, scene1.vector_Light[0].position.y, scene1.vector_Light[0].position.z);
	}
	*/
}

void Callback_special_keys(int key, int x, int y)
{
	if (key == GLUT_KEY_F1)
		main_scene.show_cone = !main_scene.show_cone;

	if (key == GLUT_KEY_F2)
		main_scene.show_tetrahedron = !main_scene.show_tetrahedron;


	/*
	//переключать фигуры вперед
	if (key == GLUT_KEY_UP)
	{
		if (scene1.add_del)
			scene1.switch_forward();
	}

	//переключать фигуры назад
	if (key == GLUT_KEY_DOWN)
	{
		if (scene1.add_del)
			scene1.switch_backward();
	}

	//сменить активный контейнер фигур
	if (key == GLUT_KEY_LEFT)
	{
		if (scene1.add_del)
			if (scene1.sphere_mod)
			{
				if (scene1.vector_Tetrahedron.size() > 0)
				{
					scene1.sphere_mod = false;
					scene1.tetrahedron_mod = true;
				}
			}
			else
			{
				if (scene1.vector_Sphere.size() > 0)
				{
					scene1.sphere_mod = true;
					scene1.tetrahedron_mod = false;
				}
			}
	}*/
	glutPostRedisplay();
}

void Menu(int pos)
{
	int key = (keys)pos;

	switch (key) {
	case KeyT: Keys_('t', 0, 0); break;
	case KeyC: Keys_('c', 0, 0); break;
	case KeyE: Keys_('e', 0, 0); break;
	case KeyW: Keys_('w', 0, 0); break;
	case KeyS: Keys_('s', 0, 0); break;
	case KeyA: Keys_('a', 0, 0); break;
	case KeyD: Keys_('d', 0, 0); break;

	case Key1: Keys_('1', 0, 0); break;
	case Key2: Keys_('2', 0, 0); break;
	case Key3: Keys_('3', 0, 0); break;
	case Key4: Keys_('4', 0, 0); break;
	case Key5: Keys_('5', 0, 0); break;
	case Key6: Keys_('6', 0, 0); break;

	default:

		int menu_tras = glutCreateMenu(Menu);
		glutAddMenuEntry("Включить трассировку", KeyT);

		int menu_off = glutCreateMenu(Menu);
		glutAddMenuEntry("Включить режим исключения фигур(С)", KeyC);
		glutAddMenuEntry("Исключить фигуру(Е)", KeyE);

		int menu_show = glutCreateMenu(Menu);
		glutAddMenuEntry("Источник 1", Key1);
		glutAddMenuEntry("Источник 2", Key2);
		glutAddMenuEntry("Источник 3", Key3);
		glutAddMenuEntry("Источник 4", Key4);
		glutAddMenuEntry("Источник 5", Key5);
		glutAddMenuEntry("Источник 6", Key6);

		//Интерфейс меню
		int menu = glutCreateMenu(Menu);
		glutAddSubMenu("Освещение", menu_show);
		glutAddSubMenu("Трассировка лучей", menu_tras);
		glutAddSubMenu("Включить/Исключить фигуру", menu_off);

		glutAttachMenu(GLUT_RIGHT_BUTTON);
		Keys_(Empty, 0, 0);
	}
}

void Mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		System_.rotation_mode = 1;
		glutSetCursor(GLUT_CURSOR_SPRAY);
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		System_.rotation_mode = 0;
		glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
	}

	glutPostRedisplay();
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0) {
		spect.spectator_move(0.1);
	}
	else {
		spect.spectator_move(-0.1);
	}

	return;
}
