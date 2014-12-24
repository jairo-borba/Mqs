/*
 * 
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 jairo-borba
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "mqsProvider/MessageQueueHeader.h"
#include "mqsProvider/MessageQueueServiceServer.h"
#include "mqsProvider/MessageQueueServiceClient.h"
#include "mqsProvider/WinNamedSharedMemAllocator.h"
#include "mqsProvider/WinNamedSharedMemAccess.h"
#include "mqsProvider/Publisher.h"
#include "mqsProvider/Subscriber.h"
#include "mqsProvider/WinMutexServer.h"
#include "mqsProvider/WinMutexClient.h"
#include "mqsProvider/Controller.h"
#include "mqsProvider/Session.h"
#include "mqsProvider/ServerSession.h"
#include "appCore/OSEnvironment.h"
#include "mqsFactory\DefEnvVarsNames.h"
#include <windows.h>

void install()
{
	bool l_bRet = false;

	appCore::OSEnvironment l_env;
	char l_keyWord[1024];
	char l_shmName[1024];

	safeSPrintf(l_keyWord, sizeof l_keyWord, "%s", l_env.getRequiredEnv(ENVIRONMENT_VARIABLE_NAME_FOR_KEY_WORD).c_str());
	safeSPrintf(l_shmName, sizeof l_shmName, "Global\\%s", l_keyWord);


	mqsProvider::WinMutexServer l_mutexSrv;
	mqsProvider::WinMutexClient l_mutexCli;
	l_mutexSrv.setEventName(l_keyWord);
	l_mutexCli.setEventName(l_keyWord);

	mqsProvider::WinNamedSharedMemAllocator l_alloc;
	l_alloc.setSharedMemoryName(l_shmName);

	mqsProvider::MessageQueueServiceServer l_srv;
	l_srv
		.setName("AVL-MESSAGE QUEUE SERVICE")
		.setDescription("First Message Queue Service")
		.setId(1);

	l_srv.setSharedMemoryAllocator(&l_alloc);
	l_srv.setMutualExclusionServer(&l_mutexSrv);
	l_srv.setMutualExclusionClient(&l_mutexCli);
	printf("Enter SUCCESS\n");
	l_bRet = l_srv.create();

	system("pause");

	l_srv.header()->status = mqsProvider::CLOSING;
}
void manager(void)
{
	bool l_bRet = false;
	mqsProvider::ServerSession l_session;
	l_session.connect();
	const mqsProvider::Controller* l_ctl = l_session.attachController();
	l_ctl->createMessageQueue( "MSGQ_AVL2", 256, 3 );
	l_ctl->createMessageQueue( "MSGQ_AVL3", 256, 10 );
	
	system( "pause" );

	l_ctl->destroyMessageQueue( "MSGQ_AVL3" );

	system( "pause" );

	

	l_ctl->destroyMessageQueue( "MSGQ_AVL2" );
	l_session.disconnect();

	system( "pause" );
}

void sender(void)
{
	mqsProvider::Session l_session;
	char message[1024];
	l_session.connect();
	
	for( int i = 0; i < 200; ++i ){
		
		scanf("%s", message);
		mqsProvider::Publisher::SEND_STATUS l_status =
			l_session.attachPublisher("MSGQ_AVL2")->identifyMessage("").sendf("Message Received: %s", message);
			
			//.sendf( "B%s.%s - %d", "jairo", "borba", i );
		switch( l_status )
		{
		case mqsProvider::Publisher::SND_EASY_SUCCESS:
			printf( "+" );
			break;
		case mqsProvider::Publisher::SND_ON_THROTLING_SUCCESS:
			printf( "Send success, but on throtling(%d)\n", i );
			break;
		case mqsProvider::Publisher::SND_MISSING_HEADER:
			printf( "SND_MISSING_HEADER, message dropped(%d)\n", i );
			break;
		case mqsProvider::Publisher::SND_MISSING_TREE_NODE:
			printf( "SND_MISSING_TREE_NODE, message dropped(%d)\n", i );
			break;
		case mqsProvider::Publisher::SND_MISSING_CLUSTER:
			printf( "SND_MISSING_CLUSTER, message dropped(%d)\n", i );
			break;
		case mqsProvider::Publisher::SND_FATAL_ERROR:
			printf( "FATAL ERROR(%d)\n", i );
			return;
		case mqsProvider::Publisher::SND_INPUT_OFF:
			printf( "(input off)\n" );
			break;
		case mqsProvider::Publisher::SND_MSGQUEUE_DESTROYED:
			printf( "RECV_MSGQUEUE_DESTROYED\n" );
			return;
		case mqsProvider::Publisher::SND_SYSTEM_DOWN:
			printf( "SND_SYSTEM_DOWN\n" );
			return;
		case mqsProvider::Publisher::SND_COUNT_LIMIT:
			printf( "LIMIT\n" );
			break;
		}
	}
}

void receiver(void)
{
	mqsProvider::Session l_session;
	l_session.connect();

	const mqsProvider::Subscriber* l_sub = l_session.attachSubscriber( "MSGQ_AVL2" );
	mqsProvider::Subscriber::RECEIVE_STATUS l_status = l_sub->identifyMessage("").receive();
	while( l_status == mqsProvider::Subscriber::RECV_SUCCESS ){
		printf( "Msg:<%s><elapsedTime<%d>\n", l_sub->message(), l_sub->elapsedTime() );
		l_status = l_sub->identifyMessage("").receive();
	}

	switch( l_status )
	{
	case mqsProvider::Subscriber::RECV_FATAL_ERROR:
		printf( "RECV_FATAL_ERROR\n" );
		break;
	case mqsProvider::Subscriber::RECV_SUCCESS:
		printf( "RECV_SUCCESS\n" );
		break;
	case mqsProvider::Subscriber::RECV_MSGQUEUE_DESTROYED:
		printf( "RECV_MSGQUEUE_DESTROYED\n" );
		break;
	case mqsProvider::Subscriber::RECV_TIMEOUT:
		printf( "RECV_TIMEOUT\n" );
		break;
	case mqsProvider::Subscriber::RECV_NO_MESSAGE:
		printf( "RECV_NO_MESSAGE\n" );
		break;
	case mqsProvider::Subscriber::RECV_SYSTEM_DOWN:
		printf( "RECV_SYSTEM_DOWN\n" );
		break;
	}
}

int main(int argc, char*argv[])
{
	if (argc < 2) return -1;

	std::string l_param(argv[1]);
	try
	{
		if (l_param == "inst")
		{
			install();
		}
		else if (l_param == "mng")
		{
			manager();
		}
		else if (l_param == "snd")
		{
			sender();
		}
		else if (l_param == "recv")
		{
			receiver();
		}
		else
		{
			printf("Wrong option\n");
		}
	}	catch( base::ExceptionInfo& e ) {
		printf( "Exception : %s at %s:%d\n", e.what(), e.file(), e.line() );
	}

	printf("Goodbye\n");
	system( "pause" );

	return 0;
}
