#ifndef CONSTRUCTS_H_
#define CONSTRUCTS_H_

// custom paletted cameos
// TODO: add a static vector to buffer instances of the same Palette file?
#include <ConvertClass.h>
#include <ScenarioClass.h>
#include <Theater.h>
#include <CCINIClass.h>
#include <GeneralStructures.h>
#include <StringTable.h>

#include <cstring>

#include "../Ares.h"
#include "../Ares.CRT.h"

class CustomPalette {
public:
	ConvertClass *Convert;
	BytePalette *Palette;

	CustomPalette() :
		Convert(NULL),
		Palette(NULL)
	{};

	~CustomPalette() {
		GAME_DEALLOC(this->Convert);
		GAME_DEALLOC(this->Palette);
	}

	bool LoadFromINI(CCINIClass *pINI, const char *pSection, const char *pKey, const char *pDefault="") {
		if(pINI->ReadString(pSection, pKey, pDefault, Ares::readBuffer, Ares::readLength)) {
			if(char * suffix = strstr(Ares::readBuffer, "~~~")) {
				const char * theaterSpecific = Theater::Array[ScenarioClass::Instance->Theater].Extension;
				suffix[0] = theaterSpecific[0];
				suffix[1] = theaterSpecific[1];
				suffix[2] = theaterSpecific[2];
			}
			GAME_DEALLOC(this->Palette);
			GAME_DEALLOC(this->Convert);
			this->Palette = NULL;
			this->Convert = NULL;
			ConvertClass::CreateFromFile(Ares::readBuffer, &this->Palette, &this->Convert);
			return !!this->Convert;
		}
		return false;
	};
};

// vector of char* with builtin storage
template<class T>
class VectorNames {
protected:
	DynamicVectorClass<char *> Strings;
	char * Buffer;

public:
	char* operator [](int Index) {
		if(Index < 0 || Index > this->Strings.Count) {
			return NULL;
		}
		return this->Strings.GetItem(Index);
	}

	T * FindItem(int Index) {
		return T::Find((*this)[Index]);
	}

	const DynamicVectorClass<char *> & Entries() const{
		return this->Strings;
	}

	char ** ToString() {
		return this->Strings.Items;
	}

	int Count() const {
		return this->Strings.Count;
	}

	VectorNames<T>(const char * Buf = NULL) {
		this->Buffer = _strdup(Buf);
	}

	void Tokenize(const char * Buf = NULL) {
		if(Buf) {
			if(this->Buffer) {
				free(this->Buffer);
			}
			this->Buffer = _strdup(Buf);
		}
		this->Strings.Clear();

		char* context = nullptr;
		for(char * cur = strtok_s(this->Buffer, ",", &context); cur && *cur; cur = strtok_s(nullptr, ",", &context)) {
			this->Strings.AddItem(cur);
		}
	}
};

// provides storage for a csf label with automatic lookup.
class CSFText {
public:
	CSFText(const char* label = nullptr) : Text(nullptr) {
		*this = label;
	}

	const CSFText& operator = (const char* label) {
		this->Label[0] = 0;
		this->Text = nullptr;

		if(label && *label) {
			if(this->Label != label) {
				AresCRT::strCopy(this->Label, label, 0x20);
			}
			this->Text = StringTable::LoadString(this->Label);
		}

		return *this;
	}

	const CSFText& operator = (const CSFText& other) {
		if(this != &other) {
			AresCRT::strCopy(this->Label, other.Label, 0x20);
			this->Text = other.Text;
		}

		return *this;
	}

	operator const wchar_t* () const {
		return this->Text;
	}

	bool empty() const {
		return !this->Text || !*this->Text;
	}

	char Label[0x20];
	const wchar_t* Text;
};

#endif
