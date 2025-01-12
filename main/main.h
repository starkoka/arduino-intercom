#define DISCORD_WEBHOOKPORT 443
#define SWITCH_PIN 10
#define SPEAKER_PIN 12

//マトリックスパネルのステータス設定
#define _STATE_STANDBY_ 0
#define _STATE_POSTTING_ 1
#define _STATE_RINGING_ 2
#define _STATE_OK_ 3
#define _STATE_DONT_ENTRY_ 4
#define _STATE_NG_ 5
#define _STATE_WAIT_ 6

void matrixTimer(timer_callback_args_t *arg);
unsigned long ck = 0;
int matrixState = 0;

void sendWebhook();

const uint32_t animation[][4] = {
	{
		0x0,
		0x0,
		0x0,
		66
	},
	{
		0xf00f01f,
		0x81f83fc3,
		0xfc090060,
		66
	},
	{
		0x4f08f01f,
		0x81f83fc3,
		0xfc091062,
		66
	},
	{
		0xf00f01f,
		0x81f83fc3,
		0xfc090060,
		66
	},
	{
		0xf20f11f,
		0x81f83fc3,
		0xfc890460,
		66
	},
	{
		0xf00f0060,
		0x6006006,
		0x600600,
		66
	},
	{
		0xf00f0060,
		0x6006006,
		0x6c06c0,
		66
	},
	{
		0xf00f0060,
		0x6186186,
		0x186d86d8,
		66
	},
	{
		0xf03f0360,
		0x361b61b6,
		0x1b6db6db,
		66
	},
  {
		0x1039,
		0x24544584,
		0x54392000,
		66
	},
	{
		0x601f81f,
		0x82042041,
		0xf81f8060,
		66
	},
	{
		0x20410809,
		0x600600,
		0x90108204,
		66
	},
	{
		0x3fc20424,
		0x42442742,
		0x42043fc,
		66
	}
};