#include "Program.h"
#include <fstream>
#include <vector>

Program::Program() : _attributeCount(0), _id(0), _vertex(0), _frag(0) {}

Program::~Program() {
	if (_id) glDeleteProgram(_id);
}

void Program::compileShaders(const char* vertexFile, const char* fragFile) {
	//Vertex and fragment shaders are successfully compiled.
	//Now time to link them together into a program.
	//Get a program object.
	_id = glCreateProgram();

	//Create the vertex shader object, and store its ID
	if (!std::string(vertexFile).empty()) {
		_vertex = glCreateShader(GL_VERTEX_SHADER);
		if (!_vertex) throw;
		compileShader(vertexFile, _vertex);
	}

	//Create the fragment shader object, and store its ID
	_frag = glCreateShader(GL_FRAGMENT_SHADER);
	if (!_frag) throw;
	compileShader(fragFile, _frag);
}

void Program::compileShader(const char* fileName, GLuint id) {
	std::ifstream file(fileName, std::ios::binary);
	if (file.fail()) throw;
	std::string buffer;
	//seek to the end
	file.seekg(0, std::ios::end);

	//Get the file size
	unsigned int fileSize = (unsigned int)file.tellg();
	file.seekg(0, std::ios::beg);

	//Reduce the file size by any header bytes that might be present
	fileSize -= (unsigned int)file.tellg();

	buffer.resize(fileSize);
	file.read((char*) & (buffer[0]), fileSize);
	file.close();

	const char* bufferArr = buffer.c_str();
	//tell OpenGL that we want to use fileContents as the contents of the shader file
	glShaderSource(id, 1, &bufferArr, nullptr);

	//compile the shader
	glCompileShader(id);

	//check for errors
	GLint success = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

		//Provide the info log in whatever manor you deem best.
		//Exit with failure.
		glDeleteShader(id); //Don't leak the shader.

		//Print error log and quit
		std::printf("%s\n", &(errorLog[0]));
 		throw;
	}
}

void Program::link() {

	//Attach our shaders to our program
	glAttachShader(_id, _vertex);
	glAttachShader(_id, _frag);

	//Link our program
	glLinkProgram(_id);

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(_id, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(_id, maxLength, &maxLength, &errorLog[0]);

		//We don't need the program anymore.
		glDeleteProgram(_id);
		//Don't leak shaders either.
		glDeleteShader(_vertex);
		glDeleteShader(_frag);

		//print the error log and quit
		std::printf("%s\n", &(errorLog[0]));
		throw;
	}

	//Always detach shaders after a successful link.
	glDetachShader(_id, _vertex);
	glDetachShader(_id, _frag);
	glDeleteShader(_vertex);
	glDeleteShader(_frag);
}

void Program::addAttribute(const char* name) {
	glBindAttribLocation(_id, _attributeCount++, name);
}

GLint Program::getUniformLocation(const char* name) {
	GLint location = glGetUniformLocation(_id, name);
	//if (location == GL_INVALID_INDEX) throw;
	return location;
}

void Program::use() {
	glUseProgram(_id);
	//enable all the attributes we added with addAttribute
	for (int i = 0; i < _attributeCount; i++) {
		glEnableVertexAttribArray(i);
	}
}

void Program::unuse() {
	glUseProgram(0);
	for (int i = 0; i < _attributeCount; i++) {
		glDisableVertexAttribArray(i);
	}
}
