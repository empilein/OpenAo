USE [atum2_db_1]
GO
/****** Object:  StoredProcedure [dbo].[PB_INIT]    Script Date: 01/04/2017 11:44:29 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO


-- ÃÂ¢Â¼Ã“Ã€Ãš ÃƒÂ¼Ã…Â© ÃƒÃŠÂ±Ã¢ÃˆÂ­ Ã‡ÃÂ·ÃŽÂ½ÃƒÃÂ®
-- 2003-09-22 by CKS
ALTER PROCEDURE [dbo].[PB_INIT]
--	@nGameID int		-- GameSvr ID
AS
	--exec [pubaccount].[dbo].PB_INIT @nGameID

	-- 2007-01-29 by cmkwon, ServerGroupID ÃƒÃŠÂ±Ã¢ÃˆÂ­
		UPDATE atum2_db_account.dbo.td_account 
		SET ConnectingServerGroupID = 0
--		WHERE ConnectingServerGroupID = @nGameID
