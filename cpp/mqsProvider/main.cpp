#include "mqsProvider/MessageQueueHeader.h"
#include "mqsProvider/MessageQueueServiceServer.h"
#include "mqsProvider/MessageQueueServiceClient.h"
#include "mqsProvider/Publisher.h"
#include "mqsProvider/Subscriber.h"
#include "mqsProvider/Controller.h"
#include "mqsProvider/Session.h"
#include "mqsProvider/ServerSession.h"

#if defined(_MSC_VER)
#include <windows.h>
#include "mqsProvider/WinNamedSharedMemAllocator.h"
#include "mqsProvider/WinNamedSharedMemAccess.h"
#include "mqsProvider/WinMutexServer.h"
#include "mqsProvider/WinMutexClient.h"

void server(void)
{
	bool l_bRet = false;
	mqsProvider::WinMutexServer l_mutexSrv;
	mqsProvider::WinMutexClient l_mutexCli;
	l_mutexSrv.setEventName( "MessageQueueService" );
	l_mutexCli.setEventName( "MessageQueueService" );
	
	mqsProvider::WinNamedSharedMemAllocator l_alloc;
	l_alloc.setSharedMemoryName( "Global\\MessageQueueService" );

	//SERVER
	mqsProvider::MessageQueueServiceServer l_srv;
	l_srv
		.setName( "AVL-MESSAGE QUEUE SERVICE" )
		.setDescription( "First Message Queue Service" )
		.setId( 1 );
	
	l_srv.setSharedMemoryAllocator( &l_alloc );
	l_srv.setMutualExclusionServer( &l_mutexSrv );
	l_srv.setMutualExclusionClient( &l_mutexCli );
	printf( "Enter SUCCESS\n" );
	l_bRet = l_srv.create();

	mqsProvider::ServerSession l_session;
	l_session.connect();
	const mqsProvider::Controller* l_ctl = l_session.attachController();
	l_ctl->createMessageQueue( "MSGQ_AVL2", 256, 3 );
	l_ctl->createMessageQueue( "MSGQ_AVL3", 256, 10 );
	
	system( "pause" );

	l_ctl->destroyMessageQueue( "MSGQ_AVL3" );

	system( "pause" );

	l_srv.header()->status = mqsProvider::CLOSING;

	l_ctl->destroyMessageQueue( "MSGQ_AVL2" );
	l_session.disconnect();

	system( "pause" );
}

void sender(void)
{
	mqsProvider::Session l_session;
	l_session.connect();
	
	for( int i = 0; i < 200; ++i ){
		
		mqsProvider::Publisher::SEND_STATUS l_status = 
			l_session.attachPublisher( "MSGQ_AVL2" )->identifyMessage("").sendf( "B%s.%s - %d", "jairo", "borba", i );
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

int main(void)
{
	try
	{
		//server();
		//sender();
		receiver();
	}	catch( appUtil::JJJException& e ) {
		printf( "Exception : %s at %s:%d\n", e.what(), e.file(), e.line() );
	}

	system( "pause" );

	return 0;
}
#else
#       if defined(__GNUC__)


#include "mqsProvider/PosixSharedMemoryAllocator.h"
#include "mqsProvider/PosixSharedMemoryAccess.h"
#include "mqsProvider/PosixBinSemaphoreServer.h"
#include "mqsProvider/PosixBinSemaphoreClient.h"
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

void sender(void)
{
	mqsProvider::Session l_session;
	l_session.connect();

	for( int i = 0; i < 200; ++i ){

		mqsProvider::Publisher::SEND_STATUS l_status =
			l_session.attachPublisher( "MSGQ_AVL2" )->identifyMessage("").sendf( "B%s.%s - %d", "jairo", "borba", i );
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

void server(void)
{

	mqsProvider::PosixBinSemaphoreServer l_mutexSrv;
	mqsProvider::PosixBinSemaphoreClient l_mutexCli;

	mqsProvider::PosixSharedMemoryAllocator l_alloc;

	unixSystem::semaphore::BinarySemaphoreServer l_binSemSrv;
	l_binSemSrv.setFileToKey(std::string("/etc/passwd"));
	l_mutexSrv.setBinarySemaphoreServer(&l_binSemSrv);
	l_alloc.setFileToKey(std::string("/etc/passwd"));
	l_mutexCli.setFileToKey(std::string("/etc/passwd"));

	//SERVER
	mqsProvider::MessageQueueServiceServer l_srv;

	l_srv.setSharedMemoryAllocator( &l_alloc );
	l_srv.setMutualExclusionServer( &l_mutexSrv );
	l_srv.setMutualExclusionClient( &l_mutexCli );
	printf( "Enter SUCCESS\n" );
	if( !l_srv.create() )
	{
		printf("MessageQueueServiceServer not created\n");
	}

	printf("session\n");

	mqsProvider::ServerSession l_session;

	if( !l_session.connect() )
	{
		printf("not conected\n");
	}
	printf("controller\n");
	const mqsProvider::Controller* l_ctl = l_session.attachController();

	if(l_ctl == NULL)
	{
		printf("controller nor working\n");
	}
	printf("creating queue\n");
	l_ctl->createMessageQueue( "MSGQ_AVL2", 256, 3 );
	l_ctl->createMessageQueue( "MSGQ_AVL3", 256, 10 );

	printf("loop\n");
	for(;;)
	{
		sleep(1);
	}

	l_ctl->destroyMessageQueue( "MSGQ_AVL3" );


	l_srv.header()->status = mqsProvider::CLOSING;

	l_ctl->destroyMessageQueue( "MSGQ_AVL2" );
	l_session.disconnect();

}

int main(int argc, char *argv[])
{
	if(argc == 2 && strcmp(argv[1],"server") == 0 )
	{

		try
		{
			server();
		}
		catch(appUtil::JJJException &e)
		{
			printf("%05d:%s:%s\n", e.line(), e.file(), e.what());
		}
	}
	else if(argc == 2 && strcmp(argv[1],"rec") == 0 )
	{
		try
		{
			receiver();
		}
		catch(appUtil::JJJException &e)
		{
			printf("%05d:%s:%s\n", e.line(), e.file(), e.what());
		}
	}
	else if(argc == 2 && strcmp(argv[1],"snd") == 0 )
	{
		try
		{
			sender();
		}
		catch(appUtil::JJJException &e)
		{
			printf("%05d:%s:%s\n", e.line(), e.file(), e.what());
		}
	}
	return 0;
}

#endif
#endif

