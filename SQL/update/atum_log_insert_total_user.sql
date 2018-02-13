USE [atum2_db_account]
GO
/****** Object:  StoredProcedure [dbo].[atum_log_insert_total_user]    Script Date: 31/03/2017 19:47:36 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
ALTER PROCEDURE [dbo].[atum_log_insert_total_user]
	@i_ServerGroupName			VARCHAR(20),	
	@i_MGameServerID			INT,
	@i_UserCount				INT
--	@i_UserCountsOtherPublisherConncect	INT
AS
	INSERT INTO atum_log_total_user
	VALUES (GetDate(), @i_ServerGroupName, @i_MGameServerID, @i_UserCount, 0)


