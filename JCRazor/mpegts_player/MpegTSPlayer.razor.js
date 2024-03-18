/**
 * .NET 镜相类
 */
export class MpegTSPlayer
{
	static Create(
		media_data_source,
		config,
		video_element,
		frame_drop_log_callback_helper,
		source_open_callback_helper
	)
	{
		return new MpegTSPlayer(
			media_data_source,
			config,
			video_element,
			frame_drop_log_callback_helper,
			source_open_callback_helper
		);
	}

	/**
	 * 
	 * @param {any} media_data_source
	 * @param {any} config
	 * @param {any} video_element
	 * @param {any} frame_drop_log_callback_helper
	 * @param {any} source_open_callback_helper 源被打开时触发回调的帮助器。用来通知 .NET 需要开始读取数据了。
	 */
	constructor(
		media_data_source,
		config,
		video_element,
		frame_drop_log_callback_helper,
		source_open_callback_helper
	)
	{
		// 将形参保存到字段中
		this.media_data_source = media_data_source;
		this.config = config;
		this.video_element = video_element;
		this.frame_drop_log_callback_helper = frame_drop_log_callback_helper;
		this.source_open_callback_helper = source_open_callback_helper;

		// 构造播放器
		this.player = window.mpegts.createPlayer(this.media_data_source, this.config);

		// 把下面这行玩魔法的函数调用注释掉，就可以使用普通的加载器了。
		this.__UseCustomLoader();
		this.player.attachMediaElement(this.video_element);
		this.__ListenOnFrameDrop();
	}

	GetVideoSourceBuffer()
	{
		return this.player._msectl._sourceBuffers.video;
	}

	GetAudioSourceBuffer()
	{
		return this.player._msectl._sourceBuffers.audio;
	}

	/**
	 * 搁这玩魔法
	 */
	__UseCustomLoader()
	{
		// JSInteropLoader 的构造函数中会把它的 this 赋值给此字段。
		this._loader = null;
		this.player._config.customLoader = this.__GetJSInteropLoaderClass();
	}

	/**
	 * 在用 VLC 推 ts 文件，播放结束后从头播放时，会遇到警告信息，然后视频帧卡住，永远无法恢复。
	 * 这时候只能重启播放器。
	 */
	__ListenOnFrameDrop()
	{
		this.have_callbacked = false;
		this.on_frame_drop = (log_type, log_data) =>
		{
			if (this.have_callbacked)
			{
				return;
			}

			if (log_type == "warn" && log_data.includes("[MP4Remuxer] > Dropping 1 audio frame"))
			{
				this.have_callbacked = true;
				this.frame_drop_log_callback_helper.invokeMethodAsync("Invoke");
			}
		};

		window.mpegts.LoggingControl.addLogListener(this.on_frame_drop);
	}

	/**
	 * 检查浏览器是否支持 mpegts.js 的运行。
	 * @returns
	 */
	static is_support()
	{
		return mpegts.isSupported();
	}

	CurrentTime()
	{
		return this.video_element.currentTime;
	}

	/**
	 * 开始加载缓冲区
	 */
	Load()
	{
		this.player.load();
	}

	/**
	 * 开始播放。
	 */
	Play()
	{
		this.player.play();
	}

	/**
	 * .NET 侧将字节数组送给 loader。
	 * 
	 * @param {Uint8Array} buffer
	 */
	SendBufferToLoader(buffer, length)
	{
		if (!this._loader._onDataArrival)
		{
			return;
		}

		this._loader._onDataArrival(buffer.buffer, 0, length);
	}

	Dispose()
	{
		try
		{
			this.player.unload();
			this.player.detachMediaElement();
			this.player.destroy();
		}
		catch (e)
		{
			console.log(e);
		}
	}

	__GetBaseLoaderClass()
	{
		/*
		下面两个对象用来充当枚举。在 window.mpegts 对象中可以找到下面这两个对象。
		
		export const LoaderStatus = {
			kIdle: 0,
			kConnecting: 1,
			kBuffering: 2,
			kError: 3,
			kComplete: 4
		};
		
		export const LoaderErrors = {
			OK: 'OK',
			EXCEPTION: 'Exception',
			HTTP_STATUS_CODE_INVALID: 'HttpStatusCodeInvalid',
			CONNECTING_TIMEOUT: 'ConnectingTimeout',
			EARLY_EOF: 'EarlyEof',
			UNRECOVERABLE_EARLY_EOF: 'UnrecoverableEarlyEof'
		};
		
		* /
		
		/** 
		 * Loader has callbacks which have following prototypes:
		 *     function onDataArrival(chunk: ArrayBuffer, byteStart: number, receivedLength: number): void
		 *     function onError(errorType: number, errorInfo: {code: number, msg: string}): void
		 *     function onComplete(rangeFrom: number, rangeTo: number): void
		 */
		return class BaseLoader
		{
			/**
			 * 不用理会本类中的那些 get set 访问器。这是给外部用的接口。派生类只要处理在本构造函数中
			 * 的这些字段就好了。这些字段在这里被赋予初始值。
			 * 
			 * 然后本类中有 2 个方法，open 和 abort。派生类需要定义同名的方法来覆盖这两个方法。
			 * 
			 * @param {string} typeName 派生类将自己的名称传进来。
			 */
			constructor(typeName)
			{
				// 派生类在构造函数中调用本构造函数，将名称传进来后就不用管此字段了。
				this._type = typeName;

				// 这个字段在派生类的构造函数中就可以确定。派生类在构造函数中为它赋值。
				this._needStash = false;

				// 派生类需要实时更新此字段。
				this._status = window.mpegts.LoaderStatus.kIdle;

				// 事件回调函数字段
				this._onContentLengthKnown = null;
				this._onURLRedirect = null;
				this._onDataArrival = null;
				this._onError = null;
				this._onComplete = null;
			}

			/**
			 * 将状态标志设为空闲，并将所有回调函数属性置为 null.
			 */
			destroy()
			{
				this._status = window.mpegts.LoaderStatus.kIdle;

				// 清除所有事件回调字段
				this._onContentLengthKnown = null;
				this._onURLRedirect = null;
				this._onDataArrival = null;
				this._onError = null;
				this._onComplete = null;
			}

			/**
			 * 通过检查状态标志来判断是否正在运行。
			 * @returns
			 */
			isWorking()
			{
				return this._status === window.mpegts.LoaderStatus.kConnecting || this._status === window.mpegts.LoaderStatus.kBuffering;
			}

			get type()
			{
				return this._type;
			}

			get status()
			{
				return this._status;
			}

			get needStashBuffer()
			{
				return this._needStash;
			}

			/**
			 * 实时流不会使用此属性。
			 */
			get onContentLengthKnown()
			{
				return this._onContentLengthKnown;
			}
			set onContentLengthKnown(callback)
			{
				this._onContentLengthKnown = callback;
			}

			/**
			 * jsinterop 不需要
			 */
			get onURLRedirect()
			{
				return this._onURLRedirect;
			}
			set onURLRedirect(callback)
			{
				this._onURLRedirect = callback;
			}

			/**
			 * 用 jsinterop 从 .net 侧调用此回调函数
			 */
			get onDataArrival()
			{
				return this._onDataArrival;
			}
			set onDataArrival(callback)
			{
				this._onDataArrival = callback;
			}

			get onError()
			{
				return this._onError;
			}
			set onError(callback)
			{
				this._onError = callback;
			}

			get onComplete()
			{
				return this._onComplete;
			}
			set onComplete(callback)
			{
				this._onComplete = callback;
			}

			/**
			 * 派生类需要重写此方法。
			 * 
			 * @param {any} dataSource
			 * @param {any} range
			 */
			open(dataSource, range)
			{
				throw new Error("未实现的方法。派生类需要实现。");
			}

			/**
			 * 派生类需要重写此方法。
			 */
			abort()
			{
				throw new Error("未实现的方法。派生类需要实现。");
			}
		}
	}

	/**
	 * 运行时定义一个 JSInteropLoader 类。并且会用闭包捕获本对象的 this，存到 projection_this
	 * 变量中。需要记住：每次调用此函数返回的都不是同一个类型，而是不同的类型。相当于运行时浏览器帮你
	 * 复制粘贴一份代码，既然是两处的代码，自然也就是不同的类型了。
	 * 
	 * @returns
	 */
	__GetJSInteropLoaderClass()
	{
		let projection_this = this;

		return class JSInteropLoader extends this.__GetBaseLoaderClass()
		{
			static isSupported()
			{
				return true;
			}

			constructor()
			{
				// 会设置名称字段，状态字段，并将回调函数字段设置为 null.
				super("JSInteropLoader");
				this.TAG = "JSInteropLoader";
				this._needStash = false;

				projection_this._loader = this;
				/*
		
					// 派生类需要实时更新此字段。
					this._status = window.mpegts.LoaderStatus.kIdle;
		
					// 事件回调函数字段
					this._onContentLengthKnown = null;
					this._onURLRedirect = null;
					this._onDataArrival = null;
					this._onError = null;
					this._onComplete = null;
		
				*/
			}

			destroy()
			{
				this.abort();
				super.destroy();
			}

			/**
			 * 通知 .NET 要开始读取了。
			 * 
			 * @param {any} data_source
			 */
			open(data_source)
			{
				projection_this.source_open_callback_helper.invokeMethodAsync("Invoke");
			}

			/**
			 * 通知 .NET 停止读取。
			 */
			abort()
			{
				console.log("abort 被调用，需要终止");
			}
		}
	}
}