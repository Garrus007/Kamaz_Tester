#ifndef  __INPUTSTORE_H__
#define  __INPUTSTORE_H__

#include <vector>
#include <exception>

/*
	��������� ������� ��������� �������� ������� ����������,
	������� �� ������� ������� ��������� ��������
*/
template <class T>
class InputStore
{
public:

	//������� ���
	void Reset()
	{
		_currentStage = 0;
		_inputs.clear();
	}

	// ���������� ��������� ��������
	T& GetNext()
	{
		if (_currentStage >= _inputs.size())
			throw std::exception("Too many gets");

		return _inputs[_currentStage++];
	}

	// ��������� ����� ��������
	void AddInput(T input)
	{
		_inputs.push_back(input);
	}

private:
	std::vector<T> _inputs;
	int _currentStage;
};

#endif 

