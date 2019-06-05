#include "Client.h"
#include "JFC/JApplication.h"
#include "UI/FormManager.h"
#include "../Public/Logging.h"
#include "../Public/Exception.h"

using namespace JFC;
using namespace UI;
using namespace PUBLIC;

int main()
{
	try
	{
		Singleton<Client>::Instance();
	}
	catch (Exception& e)
	{
		LOG_INFO << e.what();
		system("pause");
		return 1;
	}

	JApplication app;

	JForm* form = Singleton<FormManager>::Instance().Get("LoginForm");
	form->Show();
	
	return app.Exec();	// ÏûÏ¢Ñ­»·
}
