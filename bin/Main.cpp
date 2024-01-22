#include<filesystem> //ogarnia wczytywanie plikow
namespace fs = std::filesystem;
#define GL_GLEXT_PROTOTYPES //potrzebne do zalaczenia gl\GL.h
#include<math.h>
#include"Model.h"
#include<gl/GL.h>

//rozmiary okna
const unsigned int width = 800;
const unsigned int height = 800;

float skyboxVertices[] =
{
	//koordy
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	1, 2, 6,
	6, 5, 1,
	0, 4, 7,
	7, 3, 0,
	4, 5, 6,
	6, 7, 4,
	0, 3, 2,
	2, 1, 0,
	0, 1, 5,
	5, 4, 0,
	3, 7, 6,
	6, 2, 3
};

int main()
{
	//incjalizacja glfw i ustawienie wersji opgl
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// tworzenie okna
	GLFWwindow* window = glfwCreateWindow(width, height, "SolarSystem", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Blad tworzenia okna" << std::endl;
		glfwTerminate();
		return -1;
	}
	// zmienienie kontekstu na obecne okno
	glfwMakeContextCurrent(window);
	// zaladowanie glada 
	gladLoadGL();
	// ustawienie viewportu
	glViewport(0, 0, width, height);

	// tworzenie shaderow
	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	Shader sunShader("sun.vert", "sun.frag");

	// ustawienia swiatla
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);

	//aktywacja shaderow
	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	sunShader.Activate();
	glUniform4f(glGetUniformLocation(sunShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(sunShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// wlaczenie bufferow i innych opcji
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	// tworzenie kamery 
	Camera camera(width, height, glm::vec3(-1000.0f, 1.0f, 500.0f));

	//sciezki do modeli
	std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
	std::string jupiterPath = "/resources/planets/models/jupiter/scene.gltf";
	std::string neptunePath = "/resources/planets/models/neptune/scene.gltf";
	std::string earthPath = "/resources/planets/models/earth/scene.gltf";
	std::string marsPath = "/resources/planets/models/mars/scene.gltf";
	std::string wenusPath = "/resources/planets/models/wenus/scene.gltf";
	std::string mercuryPath = "/resources/planets/models/mercury/scene.gltf";
	std::string saturnPath = "/resources/planets/models/saturn/scene.gltf";
	std::string uranPath = "/resources/planets/models/uran/scene.gltf";
	std::string sunPath = "/resources/planets/models/sun/scene.gltf";
	
	// tworzenie modeli
	Model jupiter((parentDir + jupiterPath).c_str());
	Model neptune((parentDir + neptunePath).c_str());
	Model earth((parentDir + earthPath).c_str());
	Model wenus((parentDir + wenusPath).c_str());
	Model sun((parentDir + sunPath).c_str());
	Model mars((parentDir + marsPath).c_str());
	Model uran((parentDir + uranPath).c_str());
	Model mercury((parentDir + mercuryPath).c_str());
	Model saturn((parentDir + saturnPath).c_str());

	// utworzenie vao vbo i ebo dla skyboxu
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// sciezki cubemapa dla skyboxu
	std::string facesCubemap[6] =
	{
		parentDir + "/resources/planets/skybox/right.png",
		parentDir + "/resources/planets/skybox/left.png",
		parentDir + "/resources/planets/skybox/top.png",
		parentDir + "/resources/planets/skybox/bottom.png",
		parentDir + "/resources/planets/skybox/front.png",
		parentDir + "/resources/planets/skybox/back.png",
	};

	// tworzenie obiektu cubemapu
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// podlaczenie tekstur do cubemap
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Blad ladowania tekstury: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

	//skale predkosci i odleglosci
	float scel = 100.0f;
	float pazur = 20.0f;
	float szpon = 20.0f;
	float sunrotspeed = 1.0f;
	//predkosci planet
	float mespeed = 47.4f * pow(10, -6);
	float wespeed = 35.0f * pow(10, -6);
	float easpeed = 29.8f * pow(10, -6);
	float maspeed = 24.1f * pow(10, -6);
	float juspeed = 13.1f * pow(10, -6);
	float saspeed = 9.7f * pow(10, -6);
	float urspeed = 6.8f * pow(10, -6);
	float nespeed = 5.4f * pow(10, -6);
	float metemp = 0.0f;
	float wetemp = 0.0f;
	float eatemp = 0.0f;
	float matemp = 0.0f;
	float jutemp = 0.0f;
	float satemp = 0.0f;
	float urtemp = 0.0f;
	float netemp = 0.0f;


	while (!glfwWindowShouldClose(window))
	{
		// kolor tla (szary)
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// czyszczenie buforow
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ogarnia poruszanie kamery
		camera.Inputs(window);
		// aktualizuje polozenie kamery dla shaderow
		camera.updateMatrix(45.0f, 0.1f, 1000000.0f);
		// rysowanie planet
		sun.Draw(sunShader, camera, pazur*glm::vec3(0.0f, 0.0f, 0.0f), szpon* glm::vec3(1.3f, 1.3f, 1.3f));
		mercury.Draw(shaderProgram, camera, pazur* glm::vec3(58.0f * cos(metemp += (mespeed * sunrotspeed)), 0.0f, 58.0f * sin(metemp += (mespeed * sunrotspeed))), scel* glm::vec3(0.0049f, 0.0049f, 0.0049f));
		wenus.Draw(shaderProgram, camera, pazur * glm::vec3(108.0f * cos(wetemp += (wespeed * sunrotspeed)), 0.0f, 108.0f * sin(wetemp += (wespeed * sunrotspeed))), scel* glm::vec3(0.0121f, 0.0121f, 0.0121f));
		earth.Draw(shaderProgram, camera, pazur * glm::vec3(150.0f * cos(eatemp += (easpeed * sunrotspeed)), 0.0f, 150.0f * sin(eatemp += (easpeed * sunrotspeed))), scel* glm::vec3(0.0127f, 0.0127f, 0.0127f));
		mars.Draw(shaderProgram, camera, pazur * glm::vec3(228.0f * cos(matemp += (maspeed * sunrotspeed)), 0.0f, 228.0f * sin(matemp += (maspeed * sunrotspeed))), scel* glm::vec3(0.0068f, 0.0068f, 0.0068f));
		jupiter.Draw(shaderProgram, camera, pazur * glm::vec3(778.0f * cos(jutemp += (juspeed * sunrotspeed)),0.0f, 778.0f * sin(jutemp += (juspeed * sunrotspeed))), scel * glm::vec3(0.143f, 0.143f, 0.143f));
		saturn.Draw(shaderProgram, camera, pazur * glm::vec3(1427.0f * cos(satemp += (saspeed * sunrotspeed)), 0.0f, 1427.0f * sin(satemp += (saspeed * sunrotspeed))), scel * glm::vec3(0.121f, 0.121f, 0.121f));
		uran.Draw(shaderProgram, camera, pazur * glm::vec3(2871.0f * cos(urtemp += (urspeed * sunrotspeed)), 0.0f, 2871.0f * sin(urtemp += (urspeed * sunrotspeed))), scel * glm::vec3(0.051f, 0.051f, 0.051f));
		neptune.Draw(shaderProgram, camera, pazur * glm::vec3(4498.0f * cos(netemp += (nespeed * sunrotspeed)), 0.0f, 4498.0f * sin(netemp += (nespeed * sunrotspeed))), scel * glm::vec3(0.05f, 0.05f, 0.05f));
		
		//ogarnia cubemapy
		glDepthFunc(GL_LEQUAL);

		//rysowanie skyboxu
		skyboxShader.Activate();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// powraca domysly tryb glebokosci
		glDepthFunc(GL_LESS);

		// daje bufor do rysowania
		glfwSwapBuffers(window);
		// ogarnia input
		glfwPollEvents();
	}

	// usuwanie shaderow okna i glfw
	shaderProgram.Delete();
	skyboxShader.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}