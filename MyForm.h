#pragma once
#include <fstream>

struct Cell {
	bool* allowed; //���������� �������� � ������ �� 1 �� 9 (� ��������� �� +1, ������ �� 0 �� 8), true - ����� ��������� � ������
	char count; //���������� ���������� �������� ��� ������
	char value; //�������� � ������, -1 - �������� ��� (�������� �� 0 �� 8 � ����������� � ��������)
};

namespace SudokuForm {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// ������ ��� MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			Width = 400;
			Height = 500; 
		}
	private: bool exist = 0;
	private: int Size;
	private: int shiftVertical;
	private: int shiftHorizontal;
	private: Button^ buttonSolve = gcnew Button(); 
	private: bool solve = 1;
	private: Button^ buttonSolution = gcnew Button();
	private: bool solution = 0;
	private: Button^ buttonNext = gcnew Button();
	private: bool next = 0;
	private: Button^ buttonNewGame = gcnew Button();
	private: bool newGame = 1; 
	private: Button^ buttonHint = gcnew Button();
	private: bool hint = 1; 
	private: TextBox^ Header = gcnew TextBox();

	private: bool error = 0;

	private: Cell** Board = NULL;
	private: array<array<TextBox^>^>^ textBoxes;
	private: int** temp = NULL;
	private: int shift;
	private: int prev_i = -1; 
	private: int prev_j = -1;

	protected:
		/// <summary>
		/// ���������� ��� ������������ �������.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				std::remove("buf.bin");
				delete components;
			}
		}
		void AddSolve(float leftShift){
			this->buttonSolve->Location = System::Drawing::Point(this->ClientSize.Width * leftShift, Size * 0.93 + shiftVertical);
			this->buttonSolve->Size = System::Drawing::Size(this->ClientSize.Width * 0.2, this->ClientSize.Height * 0.06);
			this->buttonSolve->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.025), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->buttonSolve->Text = L"Solve";
			this->buttonSolve->UseVisualStyleBackColor = true;
			this->Controls->Add(buttonSolve);
		}
		void AddSolution(){
			this->buttonSolution->Location = System::Drawing::Point(this->ClientSize.Width * 0.55, Size * 0.93 + shiftVertical);
			this->buttonSolution->Size = System::Drawing::Size(this->ClientSize.Width * 0.2, this->ClientSize.Height * 0.06);
			this->buttonSolution->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.025), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->buttonSolution->Text = L"Solution";
			this->buttonSolution->UseVisualStyleBackColor = true;
			this->Controls->Add(buttonSolution);
		}
		void AddNewGame(float leftShift){
			this->buttonNewGame->Location = System::Drawing::Point(this->ClientSize.Width * leftShift, Size * 0.93 + shiftVertical);
			this->buttonNewGame->Size = System::Drawing::Size(this->ClientSize.Width * 0.2, this->ClientSize.Height * 0.06);
			this->buttonNewGame->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.022), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->buttonNewGame->Text = L"New game";
			this->buttonNewGame->UseVisualStyleBackColor = true;
			this->Controls->Add(buttonNewGame);
		}
		void AddNext(){
			this->buttonNext->Location = System::Drawing::Point(this->ClientSize.Width * 0.55, Size * 0.93 + shiftVertical);
			this->buttonNext->Size = System::Drawing::Size(this->ClientSize.Width * 0.2, this->ClientSize.Height * 0.06);
			this->buttonNext->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.025), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->buttonNext->Text = L"Next";
			this->buttonNext->UseVisualStyleBackColor = true;
			this->Controls->Add(buttonNext);
		}
		void AddHeader() {
			Header->AutoSize = false;
			Header->Top = 0.03 * Size;
			Header->Left = shiftHorizontal + 0.05 * Size;
			Header->Height = 0.2 * Size;
			Header->Width = 0.9 * Size;
			Header->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			Header->Multiline = true;
			this->Controls->Add(Header);
		}
		void AddHint() {
			this->buttonHint->Location = System::Drawing::Point(this->ClientSize.Width * 0.4, Size * 0.93 + shiftVertical);
			this->buttonHint->Size = System::Drawing::Size(this->ClientSize.Width * 0.2, this->ClientSize.Height * 0.06);
			this->buttonHint->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.025), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->buttonHint->Text = L"Hint";
			this->buttonHint->UseVisualStyleBackColor = true;
			this->Controls->Add(buttonHint);
		}
		void Clean() {
			for (int i = 0; i < 9; i++)
				for (int j = 0; j < 9; j++)
					textBoxes[i][j]->Text = "";
		}
		void CleanColor() {
			if (prev_i != -1)
				if (prev_j % 3 < 2) textBoxes[prev_i][prev_j]->BackColor = textBoxes[prev_i][prev_j + 1]->BackColor;
				else textBoxes[prev_i][prev_j]->BackColor = textBoxes[prev_i][prev_j - 1]->BackColor;
		}
		void SolutionStepDescription(int flag, int i, int j, int value) {
			Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.04), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			switch (flag) {
			case 0: {
				Header->Text = "� ������ (" + (i + 1).ToString() + "; " + (j + 1).ToString() +
					") ����� ��������� ������ ����� " + (value + 1).ToString();
				break;
			}
			case 1: {
				Header->Text = "� ������ " + (i + 1).ToString() + " ����� " + (value + 1).ToString() +
					" ����� ��������� ������ � ������� " + (j + 1).ToString();
				break;
			}
			case 2: {
				Header->Text = "� ������� " + (j + 1).ToString() + " ����� " + (value + 1).ToString() +
					" ����� ��������� ������ � ������ " + (i + 1).ToString();
				break;
			}
			case 3: {
				Header->Text = "� �������� " + (i / 3 * 3 + j / 3 + 1) + " ����� " + (value + 1).ToString() +
					" ����� ��������� ������ � ������ (" + (i + 1).ToString() + "; " + (j + 1).ToString() + ")";
				break;
			}
			case 4: {
				Header->Text = "������ ������������. ����� � ������ (" + (i + 1).ToString() + "; " + (j + 1).ToString() +
					") ���� �������. ��� " + (value + 1).ToString();
				break;
			}
			}
		}
		void Create(){
			textBoxes = gcnew array<array<TextBox^>^, 1>(9);
			exist = 1; 
			if ((this->ClientSize.Width) > (this->ClientSize.Height) / 1.25) Size = this->ClientSize.Height / 1.25;
			else Size = this->ClientSize.Width;
			shiftHorizontal = (this->ClientSize.Width - Size) / 2;
			shiftVertical = (this->ClientSize.Height - 1.25 * Size) / 2 + 0.25 * Size;
			for (int i = 0; i < 9; i++) {
				textBoxes[i] = gcnew array < TextBox^, 1>(9);
				for (int j = 0; j < 9; j++) {
					textBoxes[i][j] = gcnew TextBox();
					textBoxes[i][j]->AutoSize = false;
					textBoxes[i][j]->Multiline = true;
					textBoxes[i][j]->MaxLength = 1;
					this->textBoxes[i][j]->Top = Size * (i * 0.1 + 0.03) + shiftVertical;
					this->textBoxes[i][j]->Left = Size * (j * 0.1 + 0.05) + shiftHorizontal;
					this->textBoxes[i][j]->Width = Size * 0.1;
					this->textBoxes[i][j]->Height = Size * 0.1;
					this->textBoxes[i][j]->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
					this->Controls->Add(textBoxes[i][j]);
					this->textBoxes[i][j]->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.05), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
						static_cast<System::Byte>(204)));
					if (temp && temp[i][j]) this->textBoxes[i][j]->Text = (temp[i][j]).ToString();
				}
				if (temp) if (temp[i]) delete[]temp[i];
			}
			if (temp) {
				delete[]temp;
				temp = NULL;
			}
			for (int i = 0; i < 9; i += 2) { //������ �� ���������
				int line = i / 3 * 3; //����� ������� ���� ��������
				for (int a = 0; a < 3; a++) {
					int column = i % 3 * 3;
					for (int b = 0; b < 3; b++) {
						textBoxes[line][column]->BackColor = System::Drawing::SystemColors::Info;
						column++;
					}
					line++;
				}
			}
			AddHeader();
			if (solve && hint)
				if (!newGame && !solution) {
					Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.035), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
						static_cast<System::Byte>(204)));
					Header->Text = "������� ������� ������ � ������� ������ 'Solve' ��� ��������� ������� ��� 'Hint' - ��� ���������";
				}
				else
					AddSolve(0.25);
			if (solution) AddSolution();
			if (newGame)
				if (hint) {
					AddSolve(0.1);
					AddNewGame(0.7);
					AddHint();
				}
				else
					if (next || solution) AddNewGame(0.25);
					else AddNewGame(0.55);
			if (next) AddNext();
			if (prev_i != -1) textBoxes[prev_i][prev_j]->BackColor = System::Drawing::Color::Aquamarine;
			if (newGame) {
				if (solution) {
					Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.035), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
						static_cast<System::Byte>(204)));
					Header->Text = "������� 'Solution', ����� ������� ���� �������, ��� 'New Game', ����� ������ ����� �������";
				}
				else 
					if (next) {
						std::ifstream fin("buf.bin", std::ios::binary);
						fin.seekg((shift - 4) * sizeof(int), std::ios::beg);
						int flag, i, j, value;
						fin.read((char*)&flag, sizeof(int));
						fin.read((char*)&i, sizeof(int));
						fin.read((char*)&j, sizeof(int));
						fin.read((char*)&value, sizeof(int));
						SolutionStepDescription(flag, i, j, value);
					}
					else
						if (hint) {
							Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.035), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
								static_cast<System::Byte>(204)));
							Header->Text = "������� ������� ������ � ������� ������ 'Solve' ��� ��������� ������� ��� 'Hint' - ��� ���������";
						}
						else {
							Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.035), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
								static_cast<System::Byte>(204)));
							Header->Text = "��� ������������� ������� ������. ������� 'New Game', ����� ������ ����� �������";
						}
			}
			if (error) {
				Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.028), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					static_cast<System::Byte>(204)));
				Header->Text = "��� ������ ������������� � �� ����� �������. �������� ������� � ������� 'Solve' ��� ������� " +
					"'New Game', ����� ������ ����� �������";
			}
			if (next && prev_i == -1) {
				Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.035), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					static_cast<System::Byte>(204)));
				Header->Text = "��������� 'Next', ����� ������� ��������� ��� �������, ��� 'New Game', ����� ������ ����� �������";
			}
			if (hint && prev_i != -1) {
				std::ifstream fin("buf.bin", std::ios::binary);
				fin.seekg(0, std::ios::beg);
				int flag, i, j, value;
				fin.read((char*)&flag, sizeof(int));
				fin.read((char*)&i, sizeof(int));
				fin.read((char*)&j, sizeof(int));
				fin.read((char*)&value, sizeof(int));
				SolutionStepDescription(flag, i, j, value);
			}
		}
		void Delete() {
			if (exist) {
				this->Controls->Remove(Header);
				if (next) this->Controls->Remove(buttonNext);
				if (solution) this->Controls->Remove(buttonSolution);
				if (solve) this->Controls->Remove(buttonSolve);
				if (newGame) this->Controls->Remove(buttonNewGame);
				if (hint) this->Controls->Remove(buttonHint);
				temp = new int*[9]; 
				for (int i = 0; i < 9; i++) {
					temp[i] = new int[9];
					for (int j = 0; j < 9; j++)
						if (textBoxes[i][j]->Text != "")
							if (textBoxes[i][j]->Text == "1" || textBoxes[i][j]->Text == "2" || textBoxes[i][j]->Text == "3" ||
								textBoxes[i][j]->Text == "4" || textBoxes[i][j]->Text == "5" || textBoxes[i][j]->Text == "6" ||
								textBoxes[i][j]->Text == "7" || textBoxes[i][j]->Text == "8" || textBoxes[i][j]->Text == "9")
								temp[i][j] = Convert::ToInt16(this->textBoxes[i][j]->Text);
							else temp[i][j] = 0;
						else temp[i][j] = 0; 
				}
				for (int i = 0; i < 9; i++) {
					for (int j = 0; j < 9; j++)
						this->Controls->Remove(textBoxes[i][j]);
					if (textBoxes) if (textBoxes[i]) delete[]textBoxes[i];
				}
				if (textBoxes) delete[]textBoxes;
				exist = 0;
			}
		}
		void ChangeOfSize(System::Object^sender, System::EventArgs^ e) {
			if (ClientSize.Width != 0) {
				Delete();
				Create();
			}
			
		}
		void buttonSolve_Click(System::Object^ sender, System::EventArgs^ e) {
			CleanColor();
			if (error) error = 0;
			Cell** Board;
			Create(Board);
			std::ofstream buf("buf.bin", std::ios::binary);
			error = !Fill(Board);
			for (int i = 0; i < 9; i++)
				for (int j = 0; j < 9; j++) {
					int x = Board[i][j].value + 1;
					buf.write((char*)&x, sizeof(int));
				}
			error = error || !Selection(Board, buf);
			Delete(Board);
			buf.close();
			if (error) {
				newGame = 1; 
				if (Header->Text != "�������� �������") {
					Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.028), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
						static_cast<System::Byte>(204)));
					Header->Text = "��� ������ ������������� � �� ����� �������. �������� ������� � ������� 'Solve' ��� ������� " +
						"'New Game', ����� ������ ����� �������";
				}
			}
			else {
				hint = 0;
				this->Controls->Remove(buttonHint);
				for (int i = 0; i < 9; i++)
					for (int j = 0; j < 9; j++)
					textBoxes[i][j]->Text = (Board[i][j].value + 1).ToString();
					solution = 1;
					newGame = 1; 
					solve = 0;
					this->Controls->Remove(buttonSolve);					
					AddSolution();
					AddNewGame(0.25);
					Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.035), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
						static_cast<System::Byte>(204)));
					Header->Text = "������� 'Solution', ����� ������� ���� �������, ��� 'New Game', ����� ������ ����� �������";
			}
		}
		void buttonSolution_Click(System::Object^ sender, System::EventArgs^ e) {
			std::ifstream fin;
			fin.open("buf.bin", std::ios::binary);
			for (int i = 0; i < 9; i++)
				for (int j = 0; j < 9; j++) {
					int x; 
					fin.read((char*)&x, sizeof(int));
					if (x > 0) textBoxes[i][j]->Text = x.ToString();
					else textBoxes[i][j]->Clear();
				}					
			shift = 81;
			Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.035), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			Header->Text = "��������� 'Next', ����� ������� ��������� ��� �������, ��� 'New Game', ����� ������ ����� �������";
			solve = 0;
			solution = 0;
			next = 1;
			this->Controls->Remove(buttonSolve);
			this->Controls->Remove(buttonSolution);
			AddNext();
		}
		void buttonNext_Click(System::Object^ sender, System::EventArgs^ e) {
			CleanColor();
			std::ifstream fin("buf.bin", std::ios::binary);
			fin.seekg(shift * sizeof(int), std::ios::beg);
			int flag, i, j, value;
			fin.read((char*)&flag, sizeof(int));
			fin.read((char*)&i, sizeof(int));
			fin.read((char*)&j, sizeof(int));
			fin.read((char*)&value, sizeof(int));
			shift += 4;
			if (i > -1 && i < 9 && j > -1 && j < 9) {
				SolutionStepDescription(flag, i, j, value);
				textBoxes[i][j]->BackColor = System::Drawing::Color::Aquamarine;
				textBoxes[i][j]->Text = (value + 1).ToString();
				prev_i = i;
				prev_j = j;
			}
			else {
				next = 0;
				this->Controls->Remove(buttonNext);
				newGame = 1; 
				AddNewGame(0.4);
				Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.035), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					static_cast<System::Byte>(204)));
				Header->Text = "��� ������������� ������� ������. ������� 'New Game', ����� ������ ����� �������";
				prev_i = -1; 
				prev_j = -1;
			}
		}
		void buttonNewGame_Click(System::Object^ sender, System::EventArgs^ e) {
			Clean();
			CleanColor();
			prev_i = -1; 
			prev_j = -1;
			this->Controls->Remove(buttonNewGame);
			AddNewGame(0.7);
			if (next) {
				next = 0; 
				this->Controls->Remove(buttonNext);
			}
			if (solution) {
				solution = 0; 
				this->Controls->Remove(buttonSolution);
			}
			if (solve) this->Controls->Remove(buttonSolve);
			if (!hint) AddHint();
			hint = 1;
			AddSolve(0.1);
			Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.035), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			Header->Text = "������� ������� ������ � ������� ������ 'Solve' ��� ��������� �������  ��� 'Hint' - ��� ���������";
			solve = 1;
		}
		void buttonHint_Click(System::Object^ sender, System::EventArgs^ e){
			CleanColor();
			Cell** Board;
			Create(Board);
			std::ofstream buf("buf.bin", std::ios::binary);
			error = !Fill(Board);
			error = error || !Selection(Board, buf);
			Delete(Board);
			buf.close();
			if (error) {				
				if (Header->Text != "�������� �������") {
					Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.028), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
						static_cast<System::Byte>(204)));
					Header->Text = "������ �� ����� �������. �������� ������� ��� ������� 'New Game', ����� ������ ����� �������";
				}
			}
			else {
				Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.035), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					static_cast<System::Byte>(204)));
				Header->Text = "������� 'Hint', ����� ������� ��������� ���������, ��� ��������� ����� ���� �������";
			}
			std::ifstream fin("buf.bin", std::ios::binary);
			int flag, i, j, value;
			fin.read((char*)&flag, sizeof(int));
			fin.read((char*)&i, sizeof(int));
			fin.read((char*)&j, sizeof(int));
			fin.read((char*)&value, sizeof(int));
			if (i > -1 && i < 9 && j > -1 && j < 9) {
				SolutionStepDescription(flag, i, j, value);
				textBoxes[i][j]->BackColor = System::Drawing::Color::Aquamarine;
				textBoxes[i][j]->Text = (value + 1).ToString();
				prev_i = i;
				prev_j = j;
			}
		}
#pragma region Sudoku
		void Create(Cell**& Board) { //������� ("������") �������� ����
			Board = new Cell * [9];
			for (int i = 0; i < 9; i++) {
				Board[i] = new Cell[9];
				for (int j = 0; j < 9; j++) {
					Board[i][j].value = -1;
					Board[i][j].allowed = new bool[9];
					for (int k = 0; k < 9; k++)
						Board[i][j].allowed[k] = true;
					Board[i][j].count = 9;
				}
			}
			return;
		}
		void Delete(Cell** Board) { //�������� ����
			if (Board) {
				for (int i = 0; i < 9; i++) {
					if (Board[i]) {
						for (int j = 0; j < 9; j++)
							if (Board[i][j].allowed) delete[]Board[i][j].allowed;
						if (Board[i]) delete[]Board[i];
					}
				}
				delete[]Board;
			}
			return;
		}
		bool Fill(Cell**& Board) { //��������� ����������; ������ ��� ������������ � �������
			for (int i = 0; i < 9; i++)
				for (int j = 0; j < 9; j++) {
					if (textBoxes[i][j]->Text != "")
						if (textBoxes[i][j]->Text == "1" || textBoxes[i][j]->Text == "2" || textBoxes[i][j]->Text == "3" ||
							textBoxes[i][j]->Text == "4" || textBoxes[i][j]->Text == "5" || textBoxes[i][j]->Text == "6" ||
							textBoxes[i][j]->Text == "7" || textBoxes[i][j]->Text == "8" || textBoxes[i][j]->Text == "9")
							Board[i][j].value = Convert::ToInt16(this->textBoxes[i][j]->Text) - 1;
						else {
							Header->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", (float)(Size * 0.05), System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
								static_cast<System::Byte>(204)));
							Header->Text = "�������� �������";
							return false;
						}
					else Board[i][j].value = -1; //���������� � ������������ � ����� �������
					if (Board[i][j].value != -1)
						if (!Add(Board, i, j)) return false; //������������ ��� ����������
				}
			return true;
		}
		bool Strikethrough(Cell** Board, int a, int b, char value) { //������������ ����� � ������ ��� �������
			if (value == Board[a][b].value) return false; //������������ � �������
			if (Board[a][b].allowed[value]) {
				Board[a][b].count--; //���������� ������������ �����������, ���� ��� �������� ��� �� ���� ����������
				Board[a][b].allowed[value] = false; //����������� �� ��������� �������� � ������ ��� �������
			}
			return true;
		}
		bool Add(Cell**& Board, int i, int j) { //���������� �������� �� ����� i,j; ������ ��� ������������ ��� ���������� - �����������, ���� ����� ���������� ������� � �������������

			char value = Board[i][j].value;
			for (int k = 0; k < j; k++) if (!Strikethrough(Board, i, k, value)) return false; //������ �� ������, k - �������
			for (int k = j + 1; k < 9; k++) if (!Strikethrough(Board, i, k, value)) return false; //������ �� ������, k - �������
			for (int k = 0; k < i; k++) if (!Strikethrough(Board, k, j, value)) return false; //������ �� �������, k - ������
			for (int k = i + 1; k < 9; k++) if (!Strikethrough(Board, k, j, value)) return false;  //������ �� �������, k - ������

			int line = i / 3 * 3; //����� ������� ���� ��������
			for (int a = 0; a < 3; a++) { //������ �� ��������
				int column = j / 3 * 3;
				for (int b = 0; b < 3; b++) {
					if (line == i && column == j) {  //����� �� ��������� ��������
						column++;
						continue;
					};
					if (value == Board[line][column].value) return false; //������������ � �������
					if (Board[line][column].allowed[value]) {
						Board[line][column].count--; //���������� ������������ �����������, ���� ��� �������� ��� �� ���� ����������
						Board[line][column].allowed[value] = false; //����������� �� ��������� �������� � ��������
					}
					column++;
				}
				line++;
			}
			return true;
		}
		bool OneValuePerPlace(Cell**& Board, bool& change, int& activity, std::ofstream& buf) { //���� ������, � ������� ������ ���� ���������� ��������
			change = false;
			for (int i = 0; i < 9; i++)
				for (int j = 0; j < 9; j++)
					if (Board[i][j].count == 1 && Board[i][j].value == -1) {//���� � ������ ��� ��� �������� � ������ ���� ����������
						for (int k = 0; k < 9; k++) {  //���� ��� 
							if (Board[i][j].allowed[k]) { //�������� ���������, ��� ������������ �� ����������� �������
								Board[i][j].value = k;
								if (!Add(Board, i, j)) return false;
								int x = 0;
								buf.write((char*)&x, sizeof(int));
								buf.write((char*)&i, sizeof(int));
								buf.write((char*)&j, sizeof(int));
								buf.write((char*)&k, sizeof(int));
								activity++;
								break;
							}
						}
						change = true;
					}

			return true;
		}
		bool OnePlacePerValue(Cell**& Board, bool& change, int& activity, std::ofstream& buf) {//���� �����, ������� ����� ��������� ������ � ���� ������ ������\�������\��������
			change = false;

			char* places = new char[9]; //������� ��� ����� ����� ��������� � ������\�������\�������

			for (int i = 0; i < 9; i++) { //������ �� �������
				for (int j = 0; j < 9; j++) places[j] = 0;
				for (int j = 0; j < 9; j++) { //������ �� �������� ������ ������
					if (Board[i][j].value == -1) //���� � ������ ��� ��� �����, �������, ����� ��� ����� ���� 
						for (int n = 0; n < 9; n++)
							if (Board[i][j].allowed[n]) places[n]++;
				}
				for (int n = 0; n < 9; n++) {
					if (places[n] == 1) //���� ����� ����������� ������ ���� ���, ����� ����� ��� ����� 
						for (int j = 0; j < 9; j++)
							if (Board[i][j].allowed[n] && Board[i][j].value == -1) { //����� ��������� � ��� �� ������
								Board[i][j].value = n;
								if (!Add(Board, i, j)) { delete[]places; return false; }
								int x = 1;
								buf.write((char*)&x, sizeof(int));
								buf.write((char*)&i, sizeof(int));
								buf.write((char*)&j, sizeof(int));
								buf.write((char*)&n, sizeof(int));
								activity++;
								change = true;
								break; //���� ��� �������, ������ ������ ��� ������
							}
				}
			}

			for (int j = 0; j < 9; j++) { //������ �� �������
				for (int i = 0; i < 9; i++) places[i] = 0;
				for (int i = 0; i < 9; i++) { //������ �� ������� ������ �������
					if (Board[i][j].value == -1) //���� � ������ ��� ��� �����, �������, ����� ��� ����� ���� 
						for (int n = 0; n < 9; n++)
							if (Board[i][j].allowed[n]) places[n]++;
				}
				for (int n = 0; n < 9; n++) {
					if (places[n] == 1) //���� ����� ����������� ������ ���� ���, ����� ����� ��� ����� 
						for (int i = 0; i < 9; i++)
							if (Board[i][j].allowed[n] && Board[i][j].value == -1) { //����� ��������� � ��� �� ������
								Board[i][j].value = n;
								if (!Add(Board, i, j)) { delete[]places; return false; }
								int x = 2;
								buf.write((char*)&x, sizeof(int));
								buf.write((char*)&i, sizeof(int));
								buf.write((char*)&j, sizeof(int));
								buf.write((char*)&n, sizeof(int));
								activity++;
								change = true;
								break; //���� ��� �������, ������ ������ ��� ������
							}
				}
			}

			for (int i = 0; i < 9; i++) { //������ �� ���������
				for (int j = 0; j < 9; j++) places[j] = 0;
				int line = i / 3 * 3; //����� ������� ���� ��������
				for (int a = 0; a < 3; a++) {
					int column = i % 3 * 3;
					for (int b = 0; b < 3; b++) {
						if (Board[line][column].value == -1)
							for (int n = 0; n < 9; n++)
								if (Board[line][column].allowed[n]) places[n]++;
						column++;
					}
					line++;
				}
				for (int n = 0; n < 9; n++) {
					if (places[n] == 1) {
						line = i / 3 * 3;
						for (int a = 0; a < 3; a++) {
							int column = i % 3 * 3;
							for (int b = 0; b < 3; b++) {
								if (Board[line][column].allowed[n] && Board[line][column].value == -1) { //����� ��������� � ��� �� ������
									Board[line][column].value = n;
									if (!Add(Board, line, column)) { delete[]places; return false; }
									int x = 3;
									buf.write((char*)&x, sizeof(int));
									buf.write((char*)&line, sizeof(int));
									buf.write((char*)&column, sizeof(int));
									buf.write((char*)&n, sizeof(int));
									activity++;
									change = true;
									break;
								}
								column++;
							}
							line++;
						}

					}
				}
			}

			delete[]places;
			return true;
		}
		void Copy(Cell**& Board_copy, Cell**& Board) { //����������� ����, ������ �������� - ����, ������ - ������
			for (int i = 0; i < 9; i++) {
				for (int j = 0; j < 9; j++) {
					Board_copy[i][j].value = Board[i][j].value;
					for (int k = 0; k < 9; k++)
						Board_copy[i][j].allowed[k] = Board[i][j].allowed[k];
					Board_copy[i][j].count = Board[i][j].count;
				}
			}
			return;
		}
		bool Selection(Cell**& Board, std::ofstream& buf) { //������ �������� - ����������� ����� �������, true - �������, false - ������������ - ������� ���
			int activity = 0; 
			bool change;
			bool Global_change; //��������� ���� �� � ����� �������
			do {
				Global_change = false;
				do {
					OneValuePerPlace(Board, change, activity, buf); //��� ������� ����� ������������ (����������������), ������� � ���������� � �� �������
					if (change) Global_change = true; //���� ���� ���� ���� ���������, ���������� ��������� ��������
				} while (change); //���� ���� ���������, ���� ����� ��������� �����
				OnePlacePerValue(Board, change, activity, buf);
				if (change) Global_change = true; //���� ���� ���� ���� ���������, ���������� ��������� ��������

			} while (Global_change);

			for (int i = 0; i < 9; i++) //���������, ��� �� ������ ���������
				for (int j = 0; j < 9; j++)
					if (Board[i][j].value == -1) { //���� ������� ������������� ������
						if (Board[i][j].count == 0) { buf.seekp((int)buf.tellp() - (4 * activity) * sizeof(int), std::ios::beg); return false; } //���� � ������ ���������� ������ ��������� ���� �� ���������� ������������� ���� ������� - ������������
						int min = Board[i][j].count; //���� ������ � ���������� ��������� ����������� ���������� ���������
						int i_min = i; int j_min = j; //�� ���� ������ ��� ������ ���������, ��������� ���� ����� ��������� ������ ����� ���
						for (int a = i + 1; a < 9; a++) {
							for (int b = j + 1; j < 9; j++) {
								if (Board[a][b].value == -1 && Board[a][b].count < min) {
									min = Board[a][b].count;
									i_min = a;
									j_min = b;
								}
								if (min == 2) break; //������ ���� ��������� �� ����� ���� �� �����, ������ ������ ��� ������
							}
							if (min == 2) break;  //������ ���� ��������� �� ����� ���� �� �����, ������ ������ ��� ������
						}
						Cell** Board_copy; //����� ����, ������� ����� �������� ���������� ���� ��������
						Create(Board_copy);
						for (int n = 0; n < 9; n++) { //���� � ��������� ������ (� ����������� ����������� ���������) ����� ��������� ��� �����, ������� ���������
							Copy(Board_copy, Board); //����������� ������ ���� � ������ ������ �����, ��� ��� ���������� ����� "���������" �������� ������� �������� Add
							if (Board_copy[i_min][j_min].allowed[n]) {
								Board_copy[i_min][j_min].value = n;
								Add(Board_copy, i_min, j_min);
								int x = 4;
								buf.write((char*)&x, sizeof(int));
								buf.write((char*)&i_min, sizeof(int));
								buf.write((char*)&j_min, sizeof(int));
								buf.write((char*)&n, sizeof(int));
								if (Selection(Board_copy, buf)) { //���� ������� �������� �������, ������� �������, ������ ���� �� �����, ����� ������������
									Delete(Board);
									Board = Board_copy;
									return true;
								}
								buf.seekp((int)buf.tellp() - 4 * sizeof(int), std::ios::beg);
							}
						}
						Delete(Board_copy); //���� �� ������� �� ���� �� ����, ������� ���, ������� ������� �������
						buf.seekp((int)buf.tellp() - 4 * activity * sizeof(int), std::ios::beg);
						return false;
					}
			int x = -1; 
			for (int i = 0; i < 4; i++) buf.write((char*)&x, sizeof(int)); //������ ��� Next
			buf.close();
			return true; //���� ��� ������ ���������
		}
#pragma endregion

	private:
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// ��������� ����� ��� ��������� ������������ � �� ��������� 
		/// ���������� ����� ������ � ������� ��������� ����.
		/// </summary>
		void InitializeComponent(void)
		{
			
			this->components = gcnew System::ComponentModel::Container();
			this->ClientSizeChanged += gcnew System::EventHandler(this, &MyForm::ChangeOfSize);
			this->buttonSolve->Click += gcnew System::EventHandler(this, &MyForm::buttonSolve_Click);
			this->buttonSolution->Click += gcnew System::EventHandler(this, &MyForm::buttonSolution_Click);
			this->buttonNewGame->Click += gcnew System::EventHandler(this, &MyForm::buttonNewGame_Click);
			this->buttonNext->Click += gcnew System::EventHandler(this, &MyForm::buttonNext_Click);
			this->buttonHint->Click += gcnew System::EventHandler(this, &MyForm::buttonHint_Click);
			this->Text = L"Sudoku";
			this->Padding = System::Windows::Forms::Padding(0);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		}
#pragma endregion
	};
}
